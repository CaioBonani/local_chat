#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#define MAXNAME 50
#define MAXCHAR 500
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
        perror("Erro No Socket!\n");
        exit(EXIT_FAILURE);
    }

    printf("Socket Criado!\n");

    int cliente_fd;

    if ((cliente_fd = connect(server, (struct sockaddr*)&server_address, sizeof(server_address))) < 0){
        perror("Falha na Conexão ");
        exit(EXIT_FAILURE);
    }

    char user[MAXNAME];
    
    printf("Digite o seu usuário: ");
    scanf("%s", user);
    send(server, user, strlen(user), 0);

    printf("PARABÉNS %s .... Você está conectado no Servidor...\n", user);
        
    char buff[MAXCHAR];
    int i = 0;

    while(1){
        printf("Digite a Mensagem: ");
        scanf("%s", buff);

        if(strcmp(buff, "Exit") == 0){
            strcpy(buff, "Desconectado do Servidor...\n");
            printf("%s\n", buff);
            send(server, buff, strlen(buff), 0);
            break;
        }

        send(server, buff, strlen(buff), 0);
         
        if(read(server, buff, MAXCHAR) > 0)    
            printf("%s\n",buff );
    }

    close(cliente_fd);

    return 0;
}