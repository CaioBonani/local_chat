#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <netinet/in.h>
#include <string.h>
#include <paths.h>

#define MAXCHAR 1024
#define MAXNAME 100
#define MAXCLIENT 10
#define PORT 8080

int clients[MAXCLIENT];
pthread_t threads[MAXCLIENT];
int num_clients = 0;
int server_fd;

void *handle_client(void *arg) {
  int client_fd = *(int *)arg;
  char buffer[MAXCHAR];

  while (1) {
      int read_size = recv(client_fd, buffer, sizeof(buffer), 0);
    
      if (read_size <= 0) {
          // Cliente desconectou ou erro
          int i;
          for (i = 0; i < num_clients; i++) {
              if (clients[i] == client_fd) {
                  memmove(&clients[i], &clients[i + 1], (num_clients - i - 1) * sizeof(clients[0]));
                  num_clients--;
              }
          }
          break;
      }

      // Transmitir a mensagem para todos os clientes conectados
      int i;
      for (i = 0; i < num_clients; i++) {
          if (clients[i] != client_fd) {
              send(clients[i], buffer, read_size, 0);
          }
      }
  }

  close(client_fd);
  pthread_exit(NULL);
}

void sigint_handler(int signal) {
    printf("\nEncerrando o servidor...\n");
    close(server_fd);
    exit(0);
}

int main(){
    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)
    };

    // Inicializar o array de clientes
    int i;
    for (i = 0; i < MAXCLIENT; i++) {
        clients[i] = -1;
    }

    // Configurar o tratamento do sinal SIGINT (Ctrl+C)
    signal(SIGINT, sigint_handler);

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Erro No Socket!\n");
        exit(EXIT_FAILURE);
    }

    int addrlen = sizeof(server_address);

    if(bind(server_fd, (struct sockaddr*)&server_address, addrlen) < 0){
        perror("Falha no Bind!!");
        exit(EXIT_FAILURE);
    }

    printf("Socket vinculado na porta: %d\n", PORT);

    if(listen(server_fd, 10) < 0){
        perror("Falha no Listen!!");
        exit(EXIT_FAILURE);
    }

    printf("Listening...\n");

    struct sockaddr_in cliente_address;
    socklen_t addrlen_size;

    while (1){
      char user[MAXNAME];
      int client_fd = accept(server_fd, (struct sockaddr*)&cliente_address, &addrlen_size);
  
      if(client_fd < 0){
          perror("Falha no Accept!!");
          continue;
      }
      
      if(num_clients >= MAXCLIENT){
          printf("Número máximo de clientes alcançado. Conexão rejeitada.\n");
          close(client_fd);
          continue;
      }

      clients[num_clients] = client_fd;
      num_clients++;

      // Criar uma thread para lidar com o cliente
      if (pthread_create(&threads[num_clients - 1], NULL, handle_client, &client_fd) != 0){
          perror("Erro ao criar a thread do cliente");
          break;
      }

      recv(client_fd, user, MAXCHAR, 0);
      printf("%s Conectado!!\n", user);

      // while(1){
      //   recv(cliente_fd, buffer, MAXCHAR, 0);
        
      //   if(strcmp(buffer, "Desconectado do Servidor...")){
      //       i--;
      //   }
      // }

      // // for (int j = 0; j < i + 1; j++){
      // //     if (j != i){
      // //         send(cliente_fd[j], buffer, strlen(buffer), 0);
      // //     }
      // // }


      // if (i == 0){
      //     break;
      // }
    }

    // Fechar sockets e threads quando o servidor for encerrado
    for (i = 0; i < num_clients; i++) {
        close(clients[i]);
        pthread_cancel(threads[i]);
    }
  
    close(server_fd);
    // shutdown((server_fd), SHUT_RDWR);

    return 0;

}