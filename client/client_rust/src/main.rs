use tokio::net::TcpStream;
use tokio::io::{AsyncReadExt, AsyncWriteExt};

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let addr = "127.0.0.1:8080";
    let mut stream = TcpStream::connect(addr).await?;

    println!("Conectado a {}", addr);

    loop {
        let mut input = String::new();
        println!("Digite uma mensagem (ou 'quit' para sair): ");
        std::io::stdin().read_line(&mut input)?;

        if input.trim() == "quit" {
            break;
        }

        stream.write_all(input.as_bytes()).await?;

        let mut response = vec![0; 1024];
        stream.read(&mut response).await?;

        let response_str = String::from_utf8_lossy(&response);
        println!("Resposta do servidor: {}", response_str);
    }

    Ok(())
}
