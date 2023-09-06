#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h> // Adicionar a inclusão da biblioteca pthread

#define MAXNAME 50
#define MAXCHAR 500
#define PORT 8080

void *receive_messages(void *arg) {
    int server = *(int *)arg;
    char buff[MAXCHAR];
    
    while (1) {
        if (read(server, buff, MAXCHAR) <= 0)
            break;
        
        printf("%s\n", buff);
    }
    
    pthread_exit(NULL);
}

int main() {
    char *ip = "127.0.0.1";

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
    scanf("%s", user);
    send(server, user, strlen(user), 0);

    printf("PARABÉNS %s .... Você está conectado no Servidor...\n", user);

    pthread_t receive_thread;
    
    // Crie uma thread para receber mensagens do servidor
    if (pthread_create(&receive_thread, NULL, receive_messages, &server) != 0) {
        perror("Erro ao criar thread de recebimento");
        exit(EXIT_FAILURE);
    }

    char buff[MAXCHAR];

    while (1) {
        printf("Digite a Mensagem: ");
        fgets(buff, sizeof(buff), stdin);
        buff[strcspn(buff, "\n")] = '\0';

        if (strcmp(buff, "Exit") == 0) {
            strcpy(buff, "Desconectado do Servidor...\n");
            printf("%s\n", buff);
            send(server, buff, strlen(buff), 0);
            break;
        }

        send(server, buff, strlen(buff), 0);
    }

    // Espere a thread de recebimento terminar
    pthread_join(receive_thread, NULL);

    close(cliente_fd);

    return 0;
}
