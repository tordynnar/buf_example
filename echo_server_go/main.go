package main

import (
	"context"
	"flag"
	"fmt"
	"log"
	"net"
	"os"
	"os/signal"
	"syscall"
	"time"

	echov1 "github.com/example/buf_example/go_package/demo/echo/v1"
	"google.golang.org/grpc"
)

type echoServer struct {
	echov1.UnimplementedEchoServiceServer
}

func (s *echoServer) Echo(ctx context.Context, req *echov1.EchoRequest) (*echov1.EchoResponse, error) {
	return &echov1.EchoResponse{
		Message:   req.Message,
		Timestamp: time.Now().UTC().Format(time.RFC3339),
	}, nil
}

func (s *echoServer) EchoStream(req *echov1.EchoStreamRequest, stream grpc.ServerStreamingServer[echov1.EchoResponse]) error {
	for i := int32(0); i < req.RepeatCount; i++ {
		if err := stream.Send(&echov1.EchoResponse{
			Message:   req.Message,
			Timestamp: time.Now().UTC().Format(time.RFC3339),
		}); err != nil {
			return err
		}
		time.Sleep(100 * time.Millisecond)
	}
	return nil
}

func main() {
	port := flag.Int("port", 50051, "Port to listen on")
	flag.Parse()

	lis, err := net.Listen("tcp", fmt.Sprintf(":%d", *port))
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}

	srv := grpc.NewServer()
	echov1.RegisterEchoServiceServer(srv, &echoServer{})

	sigCh := make(chan os.Signal, 1)
	signal.Notify(sigCh, syscall.SIGINT, syscall.SIGTERM)

	go func() {
		<-sigCh
		fmt.Println("\nShutting down...")
		srv.GracefulStop()
	}()

	fmt.Printf("Echo server listening on port %d\n", *port)
	if err := srv.Serve(lis); err != nil {
		log.Fatalf("failed to serve: %v", err)
	}
}
