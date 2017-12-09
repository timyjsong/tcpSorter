#ifndef SERVER_HEADER
#define SERVER_HEADER

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "sorter.h"

char column[32];
int fileCounter = 0;

pthread_mutex_t columnMutex;

/*to keep track of the # of client requests, 
to act as counter for different sort col names */
int num_clients = -1;

/*array for sort col names, stores the column names
uses num_clients as index*/
char *sort_col_name[1000];

static void *acceptConnection(int client_fd);

static void *acceptConnection(int  client_fd){

    char buffer[32];
    int len = read(client_fd, buffer, sizeof(buffer));
    buffer[len] = '\0';

    printf("Read %d chars\n", len);

    printf("The first thing i read on this connection is: %s\n", buffer);


    //if b then create output folder, store as CSV file in output folder

    //if c then merge CSV's into one file, sort, and return large CSV

    //if a then store sort_col_name
    if(buffer[0] == 'a'){
    	num_clients++;
    	int i = 0;
    	for(i = 1; i<=sizeof(buffer); i++){
    		buffer[i-1] = buffer[i];
    	}
    	buffer[i] = '\0';
        strcpy(column, buffer);
        printf("I have received the column to sort on and it is: %s\n", column);
        sort_col_name[num_clients] = column;
        printf("sortcolname: %s\n",sort_col_name[num_clients]);
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


        char * fileName = malloc(sizeof(char) * 1024);

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
        printf("I have received a sort request, this means all the files I need are in my directory, and I must sort and send them back\n");

        sort_files_server();

        //exit(0);
    } 
}

#endif
