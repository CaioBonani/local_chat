#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

#define MAXCHAR 500
#define MAXCLIENT 5
#define MAXNAME 50
#define PORT 8080

char buffer[MAXCHAR];
int cliente_fd[MAXCLIENT];
int num_clients = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Função para tratar mensagens de um cliente
void *handle_client(void *arg) {
    int client_index = *((int *)arg);
    char buffer[MAXCHAR];

    while (1) {
        ssize_t bytes_received = recv(cliente_fd[client_index], buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            // O cliente se desconectou ou ocorreu um erro na recepção
            printf("Cliente %d desconectado.\n", client_index);
            close(cliente_fd[client_index]);
            break;
        }

        // Adicione um caractere nulo de terminação à mensagem recebida
        buffer[bytes_received] = '\0';

        // Envie a mensagem para todos os outros clientes conectados
        for (int i = 0; i < num_clients; i++) {
            if (i != client_index) {
                send(cliente_fd[i], buffer, strlen(buffer), 0);
            }
        }
    }

    // Marque a entrada como vazia
    cliente_fd[client_index] = -1;
    num_clients--;
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
        perror("Erro No Socket!\n");
        exit(EXIT_FAILURE);
    }

    int addrlen = sizeof(server_address);

    if(bind(server_fd, (struct sockaddr*)&server_address, addrlen) < 0){
        perror("Falha no Bind!!\n");
        exit(EXIT_FAILURE);
    }

    printf("Socket vinculado na porta: %d\n", PORT);

    if(listen(server_fd, MAXCLIENT) < 0){
        perror("Falha no Listen!!\n");
        exit(EXIT_FAILURE);
    }

    printf("Listening...\n");

    struct sockaddr_in cliente_address;

    socklen_t addrlen_size;

    while (1) {
        // Aceitar a conexão do cliente
        if ((cliente_fd[num_clients] = accept(server_fd, (struct sockaddr *)&cliente_address, &addrlen_size)) < 0) {
            perror("Falha no Accept!!");
            exit(EXIT_FAILURE);
        }else{
            recv(cliente_fd[num_clients], name, MAXNAME, 0);
            printf("%s Conectado!!\n", name);
        }

        // Crie uma nova thread para lidar com o cliente
        pthread_t thread;
        int *client_index = malloc(sizeof(int));

        *client_index = num_clients;
        if(pthread_create(&thread, NULL, handle_client, client_index) < 0){
            printf("Falha ao criar o Thread!!\n");
        };

        pthread_join(thread, NULL);

        // Aumente o contador de clientes
        num_clients++;
    }

    if(num_clients == 0){
        shutdown(server_fd, SHUT_RDWR);
        printf("Servidor Fechado!!\n");
    }

    return 0;
}
