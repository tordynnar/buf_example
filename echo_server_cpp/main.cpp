#include <atomic>
#include <chrono>
#include <csignal>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpcpp/grpcpp.h>

#include "demo/echo/v1/echo.grpc.pb.h"

using demo::echo::v1::EchoRequest;
using demo::echo::v1::EchoResponse;
using demo::echo::v1::EchoService;
using demo::echo::v1::EchoStreamRequest;

static std::unique_ptr<grpc::Server> g_server;
static std::atomic<bool> g_shutdown_requested{false};

static std::string utc_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf{};
    gmtime_r(&time_t_now, &tm_buf);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tm_buf);
    return buf;
}

class EchoServiceImpl final : public EchoService::Service {
    grpc::Status Echo(grpc::ServerContext* /*context*/,
                      const EchoRequest* request,
                      EchoResponse* response) override {
        response->set_message(request->message());
        response->set_timestamp(utc_timestamp());
        return grpc::Status::OK;
    }

    grpc::Status EchoStream(grpc::ServerContext* context,
                            const EchoStreamRequest* request,
                            grpc::ServerWriter<EchoResponse>* writer) override {
        for (int i = 0; i < request->repeat_count(); ++i) {
            if (context->IsCancelled()) {
                return grpc::Status(grpc::StatusCode::CANCELLED, "Client cancelled");
            }
            EchoResponse response;
            response.set_message(request->message());
            response.set_timestamp(utc_timestamp());
            writer->Write(response);
            if (i + 1 < request->repeat_count()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        return grpc::Status::OK;
    }
};

static void signal_handler(int /*signum*/) {
    g_shutdown_requested.store(true);
}

int main(int argc, char* argv[]) {
    std::string host = "0.0.0.0";
    std::string port = "50051";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "--host") && i + 1 < argc) {
            host = argv[++i];
        } else if ((arg == "--port") && i + 1 < argc) {
            port = argv[++i];
        }
    }

    std::string address = host + ":" + port;

    EchoServiceImpl service;
    grpc::ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    g_server = builder.BuildAndStart();
    if (!g_server) {
        std::cerr << "Failed to start server on " << address << std::endl;
        return 1;
    }

    std::cout << "Echo server listening on " << address << std::endl;

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::thread shutdown_thread([] {
        while (!g_shutdown_requested.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        g_server->Shutdown();
    });

    g_server->Wait();
    shutdown_thread.join();
    std::cout << "Server shut down" << std::endl;
    return 0;
}
