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
    if let Ok(bytes_read) = stream.read(&mut buffer) { // If the client sends a message and server reads it, create a string from the bytes read

        if bytes_read > 0 { 

            client_name = String::from_utf8_lossy(&buffer[..bytes_read]).trim_end_matches('\n').to_string(); // Create a string from the bytes read, trim the newline character from the end of the string
            println!("{} connected.", client_name);
        }
    }

    // Broadcast the client's name to all connected clients, scoped to a block to unlock the mutex
    {
        let mut clients = clients.lock().unwrap(); // Lock the mutex

        for client in clients.iter_mut() { //Iterate over the list of clients

            client.write_all(format!("{} connected.\n", client_name).as_bytes()).unwrap(); // Write the client's name to the client
        }

        clients.push(stream.try_clone().expect("Failed to clone stream")); // Copy the stream to the list of clients
    }

    loop {

        match stream.read(&mut buffer) { // Read the client's message

            Ok(bytes_read) => { // If the client sends a message

                if (bytes_read == 0) { //If no bytes are read, the client has disconnected

                    break;
                }

                let message = String::from_utf8_lossy(&buffer[..bytes_read]).to_string(); // Create a string from the bytes read

                println!("Received from {}: {}", client_name, message);

                broadcast_message(&clients, format!("{}: {}", client_name, message), &stream); // Broadcast the client's message to all connected clients
            }

            Err(_) => { // If there is an error reading from the client, the client has disconnected

                break;
            }
        }
    }

    // Remove the client from the list of clients
    {
        let mut clients = clients.lock().unwrap();

        clients.retain(|client| client.peer_addr().is_ok() && client.peer_addr().unwrap() != stream.peer_addr().unwrap()); //estudar

        println!("{} disconnected.", client_name);

        for client in clients.iter_mut() {

            client.write_all(format!("{} disconnected.\n", client_name).as_bytes()).unwrap();
        }
    }
}

fn broadcast_message(clients: &Arc<Mutex<Vec<TcpStream>>>, message: String, sender: &TcpStream) {

    let clients = clients.lock().unwrap();

    for mut client in clients.iter() {

        if (client.peer_addr().is_ok() && client.peer_addr().unwrap() != sender.peer_addr().unwrap()) {

            client.write_all(message.as_bytes()).unwrap();
        }
    }
}

fn main() {

    const PORTA: &str = "8080";
    // const IP: &str = "192.168.15.7";
    const IP: &str = "127.0.0.1";

    let listener = TcpListener::bind(IP.to_owned() + ":" + PORTA).expect("Failed to bind address"); // Bind the server to the address
    let clients = Arc::new(Mutex::new(Vec::new())); // Create a vector to store the clients, this vector will be shared between threads, so it needs to be wrapped in an Arc and Mutex

    println!("Server listening on {}:{}", IP, PORTA);

    for stream in listener.incoming() { // Iterate over the incoming connections

        match stream { 

            Ok(stream) => { // If the server accepts a connection

                let clients_clone = Arc::clone(&clients); // Clone the list of clients for use in the thread
                // Spawn a new thread to handle each client
                thread::spawn( move || {

                    handle_client(stream, clients_clone);
                });
            }

            Err(e) => {

                eprintln!("Error accepting client: {}", e);
            }
        }

    }
}