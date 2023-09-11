/*

    SERVER

*/

use std::io::{Read, Write};
use std::net::{TcpListener, TcpStream};
use std::thread;
use std::sync::{Arc, Mutex};
use std::time;
use chrono;

struct Client {

    name: String,
    stream: TcpStream,
}

//Function to handle each client, take the stream and the list of clients as arguments
fn handle_client(mut stream: TcpStream, all_clients: Arc<Mutex<Vec<Client>>>, now: time::Instant) {

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

        let mut all_clients = all_clients.lock().unwrap();

        for client in all_clients.iter_mut() { //Iterate over the list of clients

            client.stream.write_all(format!("{} connected.\n", client_name).as_bytes()).unwrap(); // Write the client's name to the client
        }

        all_clients.push(Client {name: client_name.clone(), stream: stream.try_clone().expect("Failed to clone stream")});
    }

    loop {

        match stream.read(&mut buffer) { // Read the client's message

            Ok(bytes_read) => { // If the client sends a message

                if (bytes_read == 0) { //If no bytes are read, the client has disconnected

                    break;
                }

                let message = String::from_utf8_lossy(&buffer[..bytes_read]).to_string(); // Create a string from the bytes read
                
                let name_clone = client_name.clone();

                println!("Received from {}: {}", name_clone, message);
              
                broadcast_message(&all_clients, name_clone, message, &stream, now); // Broadcast the client's message to all connected clients

            }

            Err(_) => { // If there is an error reading from the client, the client has disconnected
                
                break;
            }
        }
    }

    // Remove the client from the list of clients
    {
        let mut all_clients = all_clients.lock().unwrap();

        all_clients.retain(|client| client.stream.peer_addr().is_ok() && client.stream.peer_addr().unwrap() != stream.peer_addr().unwrap()); 
        all_clients.retain(|client| client.name != client_name);

        println!("{} disconnected.", client_name);

        for client in all_clients.iter_mut() {

            client.stream.write_all(format!("{} disconnected.\n", client_name).as_bytes()).unwrap();
        }
    }
}

fn broadcast_message(all_clients: &Arc<Mutex<Vec<Client>>>, client_name: String, message: String, mut sender: &TcpStream, now: time::Instant) {

    let mut all_clients = all_clients.lock().unwrap();

    let msg = message.trim();

    match msg {

        "/list" => {

            //vector to store the names of the clients
            let mut names: Vec<String> = Vec::new();

            for client in all_clients.iter(){

                let name = format!("{} - {}", client.name.to_string(), client.stream.peer_addr().unwrap().to_string()).to_string();
                names.push(name.to_string());                
            }

            let name: String = names.join("\n");

            sender.write_all(name.as_bytes()).unwrap(); // Write the client's name to the client
        }

        "/help" => {

            sender.write_all(b"[HELP] - Commands:\n").unwrap(); // Write the client's name to the client
            sender.write_all(b"\n/list - list all clients\n").unwrap(); // Write the client's name to the client
            sender.write_all(b"/time - show the time passed since the server started\n").unwrap(); // Write the client's name to the client
            sender.write_all(b"/date - show the current date and time\n").unwrap(); // Write the client's name to the client
            sender.write_all(b"/help - list all commands\n").unwrap(); // Write the client's name to the client
        }

        "/time" => {

            //show the time passed since the server started
            let time = time::Instant::now().duration_since(now).as_secs();

            let time = format!("Server running {} seconds ago. ", time);

            sender.write_all(time.as_bytes()).unwrap(); // Write the client's name to the client
        }

        "/date" => {

            let date = chrono::Local::now().format("%Y-%m-%d %H:%M:%S").to_string();

            sender.write_all(date.as_bytes()).unwrap(); // Write the client's name to the client
        }

        _ => {

            for client in all_clients.iter_mut() {

                if (client.stream.peer_addr().is_ok() && client.stream.peer_addr().unwrap() != sender.peer_addr().unwrap()) {
    
                    client.stream.write_all(format!("{}: {}",client_name, message ).as_bytes()).unwrap();
                }
            }
        }
    }
}

fn main() {

    //count time
    let now = time::Instant::now();

    const PORTA: &str = "8080";
    // const IP: &str = "192.168.15.7";
    const IP: &str = "127.0.0.1";

    let listener = TcpListener::bind(IP.to_owned() + ":" + PORTA).expect("Failed to bind address"); // Bind the server to the address

    let all_clients: Arc<Mutex<Vec<Client>>> = Arc::new(Mutex::new(Vec::new()));

    println!("Server listening on {}:{}", IP, PORTA);

    for stream in listener.incoming() { // Iterate over the incoming connections

        match stream { 

            Ok(stream) => { // If the server accepts a connection

                let all_clients_clone = Arc::clone(&all_clients);

                // Spawn a new thread to handle each client
                thread::spawn( move || {
                    
                    handle_client(stream, all_clients_clone, now);
                });
            }

            Err(e) => {

                eprintln!("Error accepting client: {}", e);
            }
        }
    }
}