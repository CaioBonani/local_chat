#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <paths.h>

#define MAXCHAR 250
#define MAXNAME 50
#define MAXCLIENT 100
#define PORT 8080

int main(){
    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)
    };

    // Configurar o tratamento do sinal SIGINT (Ctrl+C)
    signal(SIGINT, sigint_handler);   

    int server_fd;

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

    if(listen(server_fd, 5) < 0){
        perror("Falha no Listen!!");
        exit(EXIT_FAILURE);
    }

    printf("Listening...\n");

    int cliente_fd[MAXCLIENT];

    for (i = 0; i < MAX_CLIENTS; i++) {
        cliente_fd[i] = -1;
    }

    char buffer[MAXCHAR];
    char name[MAXNAME];

    struct sockaddr_in cliente_address;
    socklen_t addrlen_size;

    int i = 0;

    while (1){
        if((cliente_fd[i] = accept(server_fd, (struct sockaddr*)&cliente_address, &addrlen_size)) < 0){
            perror("Falha no Accept!!");
            continue;
            //exit(EXIT_FAILURE);
        }else{
            recv(cliente_fd[i], name, MAXCHAR, 0);
            printf("%s Conectado!!\n", name);
        }

        while (1) {
            if (recv(cliente_fd[i], buffer, MAXCHAR, 0) < 0) {
                // O cliente se desconectou ou ocorreu um erro na recepção
                printf("Erro ao receber mensagem de %s\n .....", name);
                perror(": ");
                close(cliente_fd[i]);
                break;
            }

            //printf("%s: %s\n", user, buffer);

            if(strcmp(buffer, "Desconectado do Servidor...") == 0){
                i--;
            }

            // Envie a mensagem para todos os outros clientes conectados
            for (int j = 0; j < i + 1; j++){
                if (j != i){
                    send(cliente_fd[j], buffer, strlen(buffer), 0);
                }
            }
        }

        if (i == 0){
            break;
        }

        i++;
    }

    shutdown((server_fd), SHUT_RDWR);

    return 0;

}