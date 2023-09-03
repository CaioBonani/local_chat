use tokio::net::{TcpListener, TcpStream};
use tokio::io::{AsyncReadExt, AsyncWriteExt};

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let listener = TcpListener::bind("127.0.0.1:8080").await?;
    println!("Servidor ouvindo em 127.0.0.1:8080");

    while let Ok((mut socket, _)) = listener.accept().await {
        tokio::spawn(async move {
            let mut buffer = [0; 1024];

            while let Ok(n) = socket.read(&mut buffer).await {
                if n == 0 {
                    break;
                }
                let message = String::from_utf8_lossy(&buffer[..n]);
                println!("Recebido: {}", message);

                // Enviar de volta a mensagem ao cliente
                if let Err(e) = socket.write_all(message.as_bytes()).await {
                    eprintln!("Erro ao escrever no socket: {}", e);
                    break;
                }
            }
        });
    }

    Ok(())
}
