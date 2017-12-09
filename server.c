#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>


char column[32];
int fileCounter = 0;

pthread_mutex_t columnMutex;

void *acceptConnection(void  *client_fd){
            // int n;
            // char * ready = "READY";

            // n = write(client_fd, ready, strlen(ready));

            // if(n <0){
            //     perror("write");
            // }

            char buffer[32];
            int len = read(client_fd, buffer, sizeof(buffer));
            buffer[len] = '\0';

            printf("Read %d chars\n", len);

            printf("The first thing i read on this connection is: %s\n", buffer);

            if(buffer[0] == 'a'){
                strcpy(column, buffer);
                printf("I have received the column to sort on and it is: %s\n", column);
                printf("===\n");

                return(NULL);
            }

            if(buffer[0]== 'b'){

                printf("The size of the incoming csv is: %s\n", buffer);

                memmove(buffer, buffer+1, strlen(buffer));

                printf("New buffer size: %s\n",buffer);


                char * wholeFile = malloc(atoi(buffer));
                int size = atoi(buffer);

          
                int toRead = size;
                char * filePtr = wholeFile;

                while (toRead>0){

                    int n = read(client_fd, filePtr, toRead);
                    filePtr+=n;
                    toRead -=n;

                }

                wholeFile[atoi(buffer)+1] = '\0';

                char * fileName[256];

                sprintf(fileName, "%d", fileCounter);

                strcat(fileName, ".csv");

                FILE *fp = fopen(fileName, "w+");

                if(fp){
                    fputs(wholeFile,fp);
                }
                fclose(fp);
                fileCounter++;

                printf("===\n");            
                free(wholeFile);

                return(NULL);

            }

            if(buffer[0]=='c'){
                printf("I have received a sort request, this means all the files I need are in my directory, and I must sort and send them back");
                exit(0);
            }    
        
}




int main(int argc, char **argv){
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
        pthread_join(tid,NULL);


    }

    pthread_mutex_destroy(&columnMutex);

    return 0;
}