#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXCHAR 250
#define PORT 8080

int main(){
    char *ip = "127.0.0.1";

    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(ip),
        .sin_port = htons(PORT)
    };

    int server = socket(AF_INET, SOCK_STREAM, 0);

    if(server < 0){
        printf("Erro No Socket!\n");
        exit(EXIT_FAILURE);
    }

    printf("Socket Criado!\n");

    int cliente_fd;

    if ((cliente_fd = connect(server, (struct sockaddr*)&server_address, sizeof(server_address))) < 0){
        printf("Falha na Conexão\n");
        exit(EXIT_FAILURE);
    }

    printf("Cliente conectado com o Servidor...\n");
    char buff[MAXCHAR] = "Mensagem Teste enviada pelo cliente";

    send(server, buff, strlen(buff), 0);

    printf("Cliente enviou a Mensagem........\n");
    char buffer[MAXCHAR];
    read(server, buffer, MAXCHAR);

    printf("Cliente --> Resposta Enviada Pelo Servidor = %s\n", buffer);
    close(cliente_fd);

    return 0;
}