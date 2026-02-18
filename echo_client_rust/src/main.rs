use clap::Parser;
use demo_proto_bindings::demo::echo::v1::echo_service_client::EchoServiceClient;
use demo_proto_bindings::demo::echo::v1::EchoRequest;

#[derive(Parser)]
struct Args {
    message: String,

    #[arg(long, default_value = "localhost")]
    host: String,

    #[arg(long, default_value_t = 50051)]
    port: u16,
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args = Args::parse();
    let target = format!("http://{}:{}", args.host, args.port);

    let mut client = EchoServiceClient::connect(target).await?;
    let response = client
        .echo(EchoRequest {
            message: args.message,
        })
        .await?
        .into_inner();

    println!("Response: {}", response.message);
    println!("Timestamp: {}", response.timestamp);

    Ok(())
}
