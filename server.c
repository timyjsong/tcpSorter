#include "server.h"

pthread_mutex_t columnMutex;

int main(int argc, char **argv)
{

    pthread_mutex_init(&columnMutex, NULL);

    int s;
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;


    s = getaddrinfo(NULL, "1234", &hints, &result);
    if (s != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
            exit(1);
    }

    if (bind(sock_fd, result->ai_addr, result->ai_addrlen) != 0) {
        perror("bind()");
        exit(1);
    }

    if (listen(sock_fd, 10) != 0) {
        perror("listen()");
        exit(1);
    }
    
    struct sockaddr_in *result_addr = (struct sockaddr_in *) result->ai_addr;

    printf("Listening on file descriptor %d, port %d\n", sock_fd, ntohs(result_addr->sin_port));

    printf("Waiting for connection...\n");


    while(1){

        int client_fd = accept(sock_fd, NULL, NULL);

        printf("Connection made: client_fd=%d\n", client_fd);
        pthread_t tid;
        pthread_create(&tid,NULL,acceptConnection, client_fd);
        printf("num clients: %d\n",num_clients);
        pthread_join(tid,NULL);

    }

    pthread_mutex_destroy(&columnMutex);
    
    return 0;
}