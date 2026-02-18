use std::pin::Pin;

use async_stream::stream;
use chrono::Utc;
use clap::Parser;
use demo_proto_bindings::demo::echo::v1::echo_service_server::{EchoService, EchoServiceServer};
use demo_proto_bindings::demo::echo::v1::{EchoRequest, EchoResponse, EchoStreamRequest};
use tokio::signal;
use tokio_stream::Stream;
use tonic::{Request, Response, Status};

#[derive(Parser)]
struct Args {
    #[arg(long, default_value_t = 50051)]
    port: u16,
}

#[derive(Default)]
struct EchoServiceImpl;

#[tonic::async_trait]
impl EchoService for EchoServiceImpl {
    async fn echo(&self, request: Request<EchoRequest>) -> Result<Response<EchoResponse>, Status> {
        let req = request.into_inner();
        Ok(Response::new(EchoResponse {
            message: req.message,
            timestamp: Utc::now().to_rfc3339(),
        }))
    }

    type EchoStreamStream = Pin<Box<dyn Stream<Item = Result<EchoResponse, Status>> + Send>>;

    async fn echo_stream(
        &self,
        request: Request<EchoStreamRequest>,
    ) -> Result<Response<Self::EchoStreamStream>, Status> {
        let req = request.into_inner();
        let output = stream! {
            for _ in 0..req.repeat_count {
                yield Ok(EchoResponse {
                    message: req.message.clone(),
                    timestamp: Utc::now().to_rfc3339(),
                });
                tokio::time::sleep(std::time::Duration::from_millis(100)).await;
            }
        };
        Ok(Response::new(Box::pin(output)))
    }
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args = Args::parse();
    let addr = format!("0.0.0.0:{}", args.port).parse()?;

    println!("Echo server listening on port {}", args.port);

    tonic::transport::Server::builder()
        .add_service(EchoServiceServer::new(EchoServiceImpl))
        .serve_with_shutdown(addr, async {
            signal::ctrl_c().await.ok();
            println!("\nShutting down...");
        })
        .await?;

    Ok(())
}
