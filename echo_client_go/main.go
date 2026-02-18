package main

import (
	"context"
	"flag"
	"fmt"
	"log"
	"time"

	echov1 "github.com/example/buf_example/go_package/demo/echo/v1"
	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials/insecure"
)

func main() {
	host := flag.String("host", "localhost", "Server host")
	port := flag.Int("port", 50051, "Server port")
	flag.Parse()

	args := flag.Args()
	if len(args) < 1 {
		log.Fatal("Usage: echo_client_go [flags] <message>")
	}
	message := args[0]

	target := fmt.Sprintf("%s:%d", *host, *port)
	conn, err := grpc.NewClient(target, grpc.WithTransportCredentials(insecure.NewCredentials()))
	if err != nil {
		log.Fatalf("failed to connect: %v", err)
	}
	defer conn.Close()

	client := echov1.NewEchoServiceClient(conn)
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()

	resp, err := client.Echo(ctx, &echov1.EchoRequest{Message: message})
	if err != nil {
		log.Fatalf("Echo failed: %v", err)
	}

	fmt.Printf("Response: %s\n", resp.Message)
	fmt.Printf("Timestamp: %s\n", resp.Timestamp)
}
