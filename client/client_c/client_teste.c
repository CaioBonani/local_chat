#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

void *receiveMessage(void *socket);

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024];

    // Cria um socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Erro ao criar o socket");
        exit(1);
    }

    // Configura as informações do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Conecta-se ao servidor
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erro ao conectar-se ao servidor");
        exit(1);
    }

    printf("Digite seu nome: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove a nova linha

    // Envia o nome do usuário para o servidor
    if (send(sockfd, buffer, strlen(buffer), 0) == -1) {
        perror("Erro ao enviar dados para o servidor");
        exit(1);
    }

    // Cria uma thread para receber mensagens do servidor
    pthread_t receiveThread;
    pthread_create(&receiveThread, NULL, receiveMessage, (void *)&sockfd);

    while (1) {
        //printf("Digite uma mensagem (ou 'quit' para sair): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove a nova linha

        // Sai do loop se o usuário digitar 'quit'
        if (strcmp(buffer, "quit") == 0) {
            break;
        }

        // Envia a mensagem para o servidor
        if (send(sockfd, buffer, strlen(buffer), 0) == -1) {
            perror("Erro ao enviar dados para o servidor");
            exit(1);
        }
    }

    close(sockfd);
    return 0;
}

void *receiveMessage(void *socket) {
    int sockfd = *((int *)socket);
    char buffer[1024];

    while (1) {
        ssize_t bytesReceived = recv(sockfd, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            break; // Sai do loop quando a conexão é encerrada
        }
        buffer[bytesReceived] = '\0';
        printf("%s\n", buffer);
    }

    pthread_exit(NULL);
}
