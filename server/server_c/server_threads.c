#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <paths.h>
#include <pthread.h>

#define MAXCHAR 1024
#define PORT 8080

char buffer[MAXCHAR];

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int i = 0;

void * RecebeMensagem(void *arg){
    int cliente_fd = *((int *)arg);
    recv(cliente_fd, buffer, MAXCHAR, 0);
    //read(cliente_fd, buff, MAXCHAR);
    printf("Servidor --> Requisição Feita ao Servidor n %d = %s\n", i, buffer);
    //pthread_mutex_lock(&lock);
    char buff[MAXCHAR] = "Mensagem Teste Recebida!!!";
    //pthread_mutex_lock(&lock);
    //sleep(0.3);
    printf("Servidor Respondendo o Cliente......!\n");

    send(cliente_fd, buff, strlen(buff), 0);
    //free(buff);
    printf("Exit socketThread \n");
    pthread_exit(NULL);
}

int main() {
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
        printf("Falha no Bind!!\n");
        exit(EXIT_FAILURE);
    }

    printf("Socket vinculado na porta: %d\n", PORT);

    if(listen(server_fd, 10) < 0){
        printf("Falha no Listen!!\n");
        exit(EXIT_FAILURE);
    }

    printf("Listening...\n");

    int cliente_fd;

    struct sockaddr_in cliente_address;

    socklen_t addrlen_size;

    addrlen_size = sizeof(cliente_address);

    if((cliente_fd = accept(server_fd, (struct sockaddr*)&cliente_address, &addrlen_size)) < 0){
        printf("Falha no Accept!!\n");
        exit(EXIT_FAILURE);
    }

    //Até aqui é igual, porque conecta  apenas um cliente, se fossem vários clientes,
    //a partir do listening que estaria dentro do while
    pthread_t thread_id[10];

    while(i < 10){
        if(pthread_create(&thread_id[i], NULL, RecebeMensagem, (void *)&cliente_fd) < 0){
            printf("Falha ao criar o
            Thread!!\n");
        }
        pthread_join(thread_id[i],NULL);
        i++;
    }
    
    close(cliente_fd);
    shutdown((server_fd), SHUT_RDWR);
    printf("Servidor Fechado!!\n");
    return 0;
}