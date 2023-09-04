/*

    SERVER

*/

use std::io::{Read, Write};
use std::net::{TcpListener, TcpStream};
use std::thread;
use std::sync::{Arc, Mutex};

//Function to handle each client, take the stream and the list of clients as arguments
fn handle_client(mut stream: TcpStream, clients: Arc<Mutex<Vec<TcpStream>>>) {

    let mut buffer = [0; 1024]; // Buffer to store the client's messages, 1024 bytes long
    let mut client_name = String::new(); // String to store the client's name

    // Read the client's name (the first message sent by the client)
    if let Ok(bytes_read) = stream.read(&mut buffer) {
        if bytes_read > 0 {
            client_name = String::from_utf8_lossy(&buffer[..bytes_read]).trim_end_matches('\n').to_string();
            println!("{} connected.", client_name);
        }
    }

    // Broadcast the client's name to all connected clients
    {
        let mut clients = clients.lock().unwrap();
        for client in clients.iter_mut() {
            let _ = client.write_all(format!("{} connected.\n", client_name).as_bytes());
        }
        clients.push(stream.try_clone().expect("Failed to clone stream"));
    }

    loop {
        match stream.read(&mut buffer) {
            Ok(bytes_read) => {
                if bytes_read == 0 {
                    break;
                }
                let message = String::from_utf8_lossy(&buffer[..bytes_read]).to_string();
                println!("Received from {}: {}", client_name, message);
                broadcast_message(&clients, format!("{}: {}", client_name, message), &stream);
            }
            Err(_) => {
                break;
            }
        }
    }

    // Remove the client from the list of clients
    {
        let mut clients = clients.lock().unwrap();
        clients.retain(|client| client.peer_addr().is_ok() && client.peer_addr().unwrap() != stream.peer_addr().unwrap());
        println!("{} disconnected.", client_name);
        for client in clients.iter_mut() {
            let _ = client.write_all(format!("{} disconnected.\n", client_name).as_bytes());
        }
    }
}

fn broadcast_message(clients: &Arc<Mutex<Vec<TcpStream>>>, message: String, sender: &TcpStream) {
    let clients = clients.lock().unwrap();
    for mut client in clients.iter() {

        if client.peer_addr().is_ok() && client.peer_addr().unwrap() != sender.peer_addr().unwrap() {
            let _ = client.write_all(message.as_bytes());
        }

    }
}

fn main() {
    let listener = TcpListener::bind("127.0.0.1:8080").expect("Failed to bind address");
    let clients: Arc<Mutex<Vec<TcpStream>>> = Arc::new(Mutex::new(Vec::new()));

    println!("Server listening on 127.0.0.1:8080");

    for stream in listener.incoming() {
        match stream {
            Ok(stream) => {
                let clients_clone = Arc::clone(&clients);
                // Spawn a new thread to handle each client
                thread::spawn(move || {
                    handle_client(stream, clients_clone);
                });
            }
            Err(e) => {
                eprintln!("Error accepting client: {}", e);
            }
        }
    }
}
