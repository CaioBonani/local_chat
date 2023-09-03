#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAXCHAR 250
#define PORT 8080

int i = 0;
void * clienteThreadEnviaMensagem(void *arg){
    printf("No Thread...........\n");

    int server = *((int *)arg);

    printf("Thread n %d do Cliente conectado com o Servidor...\n", i);

    char buff[MAXCHAR] = "Mensagem Teste enviada pelo cliente";
    send(server, buff, MAXCHAR, 0);
    printf("Cliente no Thread n %d enviou a Mensagem........\n", i);

    char buffer[MAXCHAR];
    read(server, buffer, MAXCHAR);
    printf("Cliente %d --> Resposta Enviada Pelo Servidor = %s\n", i, buffer);
}

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

    //Até aqui é igual, porque temos apenas um cliente, se fossem vários clientes,
    //desde o começo da main até o "Socket Criado" estariam dentro da outra função
    pthread_t thread_id[11];

    while(i < 10){
        if(pthread_create(&thread_id[i], NULL, clienteThreadEnviaMensagem, (void *)&server) != 0)
            printf("Falha ao criar o Thread!!!\n");

        sleep(1);
        i++;
    }

    int j = 0;

    while(j < 10) {
        pthread_join(thread_id[j++],NULL);
        //printf("%d:\n", j);
    }

    close(cliente_fd);
    return 0;
}