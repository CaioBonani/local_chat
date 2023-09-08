# local_chat
A simple chat for communicating locally using sockets.

To use this project follow these steps:

1. Build and Run the Rust server: 
   `cd server/server_rust && cargo run`

3. Build and Run the clients:
   
   * C: 
     `cd client/client_c && gcc client.c -o client.out && ./client.out`
   * Java: 
       `cd client/client_java && javac client.java && java client`
    * Rust: 
        `cd client/client_rust && cargo run`
   
