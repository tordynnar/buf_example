#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "demo/echo/v1/echo.grpc.pb.h"

using demo::echo::v1::EchoRequest;
using demo::echo::v1::EchoResponse;
using demo::echo::v1::EchoService;

int main(int argc, char* argv[]) {
    std::string host = "localhost";
    std::string port = "50051";
    std::string message;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "--host") && i + 1 < argc) {
            host = argv[++i];
        } else if ((arg == "--port") && i + 1 < argc) {
            port = argv[++i];
        } else if (arg[0] != '-') {
            message = arg;
        }
    }

    if (message.empty()) {
        std::cerr << "Usage: echo_client_cpp [--host HOST] [--port PORT] MESSAGE" << std::endl;
        return 1;
    }

    std::string target = host + ":" + port;
    auto channel = grpc::CreateChannel(target, grpc::InsecureChannelCredentials());
    auto stub = EchoService::NewStub(channel);

    EchoRequest request;
    request.set_message(message);

    EchoResponse response;
    grpc::ClientContext context;

    grpc::Status status = stub->Echo(&context, request, &response);
    if (!status.ok()) {
        std::cerr << "RPC failed: " << status.error_message() << std::endl;
        return 1;
    }

    std::cout << "Response: " << response.message() << std::endl;
    std::cout << "Timestamp: " << response.timestamp() << std::endl;
    return 0;
}
