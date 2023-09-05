#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <paths.h>

#define MAXCHAR 250
#define MAXCLIENT 100
#define PORT 8080

int main(){
    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)
    };

    int server_fd;

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Erro No Socket!\n");
        exit(EXIT_FAILURE);
    }

    int addrlen = sizeof(server_address);

    if(bind(server_fd, (struct sockaddr*)&server_address, addrlen) < 0){
        printf("Falha no Bind!!");
        exit(EXIT_FAILURE);
    }

    printf("Socket vinculado na porta: %d\n", PORT);

    if(listen(server_fd, 5) < 0){
        printf("Falha no Listen!!");
        exit(EXIT_FAILURE);
    }

    printf("Listening...\n");

    int cliente_fd[MAXCLIENT];
    char buffer[MAXCHAR];

    struct sockaddr_in cliente_address;
    socklen_t addrlen_size;

    int i = 0;
    while ((cliente_fd[i] = accept(server_fd,(struct sockaddr *)&cliente_address,&addrlen_size))){
        if (cliente_fd[i] < 0){
            printf("Falha no Accept!!");
            exit(EXIT_FAILURE);
        }
        printf("Cliente Conectado!!\n");
        
        recv(cliente_fd[i], buffer, MAXCHAR, 0);

        printf("Servidor --> Requisição Feita ao Servidor = %s\n", buffer);

        char buff[MAXCHAR] = "Mensagem Teste Recebida!!!";

        printf("Servidor Respondendo o Cliente......!");

        send(cliente_fd[i], buff, strlen(buff), 0);

        close(cliente_fd[i]);
        i++;
    }

    // addrlen_size = sizeof(cliente_address);

    // if((cliente_fd = accept(server_fd, (struct sockaddr*)&cliente_address, &addrlen_size)) < 0){
    //     printf("Falha no Accept!!");
    //     exit(EXIT_FAILURE);
    // }

    // recv(cliente_fd, buffer, MAXCHAR, 0);
    // //read(cliente_fd, buff, MAXCHAR);
    // printf("Servidor --> Requisição Feita ao Servidor = %s\n", buffer);
    
    // char buff[MAXCHAR] = "Mensagem Teste Recebida!!!";
    
    // printf("Servidor Respondendo o Cliente......!");
    
    // send(cliente_fd, buff, strlen(buff), 0);
    
    // close(cliente_fd);
    
    // shutdown((server_fd), SHUT_RDWR);

    return 0;

}