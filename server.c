#include "server.h"
#include "mergesort.c"

pthread_mutex_t columnMutex;

int main(int argc, char **argv)
{
    //  deal with input argument first
    int c;
    char portno[BUFFER_SIZE];
    while ((c = getopt(argc, argv, "p:")) != -1) {
        switch (c) {
            case 'p':
                strcpy(portno, optarg);
                break;
            case ':':
                fprintf(stderr, "ERROR: option -%c is missing a required argument\n", optopt);
                exit(1);
                
                break;
            case '?':
                printf("ERROR: unrecognized option\n");
                exit(1);
        }
    }

    total_movies = malloc(sizeof(movie_t));

    pthread_mutex_init(&columnMutex, NULL);

    int s;
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    s = getaddrinfo(NULL, portno, &hints, &result);
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

    if(DEBUG) printf("Listening on file descriptor %d, port %d\n", sock_fd, ntohs(result_addr->sin_port));

    if(DEBUG) printf("Waiting for connection...\n");

    while(1) {
    	socklen_t addr_size = sizeof(struct sockaddr);
    	struct sockaddr_storage their_addr;
        int client_fd = accept(sock_fd, (struct sockaddr *)&their_addr, &addr_size);


        // ADDED THIS to print ip address of client
        // Doesn't print for some reason?
        fflush(0);
        if(num_clients == -1) {
            printf("Received connections from: ");
        }
  
        struct sockaddr_in* sock4addr = (struct sockaddr_in*)&their_addr;
        struct in_addr ipAddr = sock4addr->sin_addr;

        char ip[INET_ADDRSTRLEN];

        inet_ntop(AF_INET,&ipAddr,ip,INET_ADDRSTRLEN);


        fflush(0);
        printf("%s,", ip);



        if(DEBUG) printf("Connection made: client_fd=%d\n", client_fd);
        pthread_t tid;
        pthread_create(&tid,NULL,acceptConnection, client_fd);
        if(DEBUG) printf("num clients: %d\n",num_clients);
        pthread_join(tid,NULL);

    }

    pthread_mutex_destroy(&columnMutex);
    
    return 0;
}
