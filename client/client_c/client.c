#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h> // Adicionar a inclusão da biblioteca pthread

#define MAXNAME 100
#define MAXCHAR 1024

#define PORT 39999
#define ip "172.21.219.101"
// #define ip "....."

void *receive_messages(void *socket) {
    int server = *(int *)socket;
    char buff[MAXCHAR];
    
    while (1) {
        if (recv(server, buff, sizeof(buff), 0) < 0) {
            break;
        }
        
        buff[strcspn(buff, "\n")] = '\0';
        printf("%s\n", buff);
    }
    
    pthread_exit(NULL);
}

int main() {
    
    // char *ip = "127.0.0.1";

    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(ip),
        .sin_port = htons(PORT)
    };

    int server = socket(AF_INET, SOCK_STREAM, 0);

    if (server < 0) {
        perror("Erro No Socket!\n");
        exit(EXIT_FAILURE);
    }

    printf("Socket Criado!\n");

    int cliente_fd;

    if ((cliente_fd = connect(server, (struct sockaddr*)&server_address, sizeof(server_address))) < 0) {
        perror("Falha na Conexão ");
        exit(EXIT_FAILURE);
    }

    char user[MAXNAME];

    printf("Digite o seu usuário: ");
    fgets(user, sizeof(user), stdin);
    user[strcspn(user, "\n")] = '\0';

    if(send(server, user, strlen(user), 0) < 0){
      perror("Erro ao enviar a mensagem...");
      exit(EXIT_FAILURE);
    }

    printf("%s conectado no Servidor...\n", user);

    pthread_t receive_thread;
    if (pthread_create(&receive_thread, NULL, receive_messages, &server) != 0) {
        perror("Erro ao criar thread de recebimento");
        exit(EXIT_FAILURE);
    }

    char buff[MAXCHAR];

    while (1) {
        //printf("Digite a Mensagem: ");
        fgets(buff, sizeof(buff), stdin);
        // buff[strcspn(buff, "\n")] = '\0';

        if (strcmp(buff, "Exit") == 0) {
            strcpy(buff, " Desconectado do Servidor...\n");
            printf("%s\n", buff);
          
            send(server, strcat(user, buff), strlen(buff), 0);
            break;
        }

        if(send(server, buff, strlen(buff), 0) < 0){
          perror("Erro ao enviar a mensagem...");
          exit(EXIT_FAILURE);
        }
    }

    // Espere a thread de recebimento terminar
    //pthread_join(receive_thread, NULL);
    close(cliente_fd);

    return 0;
}
