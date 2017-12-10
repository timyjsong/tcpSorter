#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>

char port[256];

char server[1024];
char finalColumn[128];

void *startFileThreaded(void * filename){

	int s;
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	struct addrinfo hints, *result;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET; 
	hints.ai_socktype = SOCK_STREAM; 

	s = getaddrinfo(server, port, &hints, &result);
	if (s != 0) {
	        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        	exit(1);
	}

	if(connect(sock_fd, result->ai_addr, result->ai_addrlen) == -1){
                perror("connect");
                exit(2);
    }

    printf("filename attempting to open: %s\n",filename);

    FILE *file = fopen(filename, "r");

    if(!file){
    	perror(NULL);
    }

    struct stat st;

    stat(filename, &st);

    printf("actual size: %d\n", st.st_size);

    int size = st.st_size;
    char  * fileSize = calloc(32,sizeof(char)*32);

    sprintf(fileSize, "%d", size);

    char firstMessage[32] = "b";
    strcat(firstMessage,fileSize);

    write(sock_fd, firstMessage, 32);

    char buffer[size];

    if(buffer){
    	fread(buffer,1,size,file);
    }


    printf("The size of the buffer i am writing to: %d \n",size);

    	int toSend =size;
    	char * filePtr = buffer;

    	while(toSend>0){
    		int n = write(sock_fd,filePtr, toSend);
    		if (n<0){
    			perror("write");
    		}

    		filePtr+=n;
    		toSend-=n;	
    	}	

    //printf("whole buffer:%s\n",buffer);

    close(sock_fd);

    free(fileSize);
    free(filename);

}

void sendColumn(char* column){

	int n;

	int s;
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	struct addrinfo hints, *result;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET; 
	hints.ai_socktype = SOCK_STREAM; 

	s = getaddrinfo(server, port, &hints, &result);
	if (s != 0) {
	        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        	exit(1);
	}

	if(connect(sock_fd, result->ai_addr, result->ai_addrlen) == -1){
                perror("connect");
                exit(2);
    }

    char  * columnToSend = calloc(32,sizeof(char)*32);

    columnToSend[0] = 'a';

    strcat(columnToSend,column);

    printf("The column i am sending is: %s\n",columnToSend);

    //char check[256];

    // printf("I am now about to wait for the server to send back ready\n");
    // while(strcmp(check, "READY")!=0){
    // 	n = read(sock_fd,check,sizeof(check)-1);
    // 	if(n<0){
    // 		perror("read");
    // 	}
    // }
    // printf("the server is ready i will now ssend column.\n");

    //check[0] = '\0';

    write(sock_fd, columnToSend, 32);

    close(sock_fd);
}



void *startDirectory(void * path){
	DIR *d;
	struct dirent *dir;

    printf("strt dir is starting with path: %s\n",path);



	d = opendir(path);

	if (d){
		while ((dir = readdir(d)) != NULL){
            char directory[1024];
            strcpy(directory, path);
			
            
			if(strcmp(dir->d_name, "..")!=0 && strcmp(dir->d_name, ".")!=0){
				if(dir->d_type == DT_DIR){
                    
					strcat(directory, "/");
					strcat(directory, dir->d_name);
					pthread_t tid;
				
					char * toEnter = malloc(sizeof(char)*1024);
					strcpy(toEnter, directory);
					pthread_create(&tid, NULL, startDirectory, toEnter);
					pthread_join(tid,NULL);

				}
				if(dir->d_type == DT_REG){
					//printf(" FILE: %s\n", dir->d_name);
					char * ptr = strchr(dir->d_name, '.');
					
					if(ptr!=NULL){
						if(strcmp(ptr,".csv")==0){
							char* file = malloc(sizeof(char)*1024);
 							strcpy(file,path);
							strcat(file, "/");
							strcat(file, dir->d_name);
							pthread_t tid;
							//printf("Creating thread to process: %s\n",file);
							pthread_create(&tid,NULL, startFileThreaded, file);
							pthread_join(tid,NULL);

						}
					}

				}
			}


		}
		closedir(d);
	}
	
	free(path);

}

void sendSortRequest(){

	

	int s;
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	struct addrinfo hints, *result;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET; 
	hints.ai_socktype = SOCK_STREAM; 

	s = getaddrinfo(server, port, &hints, &result);
	if (s != 0) {
	        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        	exit(1);
	}

	if(connect(sock_fd, result->ai_addr, result->ai_addrlen) == -1){
                perror("connect");
                exit(2);
    }

    char  * sortRequest = calloc(32,sizeof(char)*32);

    sortRequest[0] = 'c';

    write(sock_fd, sortRequest, 32);

    char fileSize[32];

    int n = read(sock_fd, fileSize, 32);

    if(n<0){
    	perror("read");
    }

    int size = atoi(fileSize);
    char  * finalFile = malloc(size);

    printf("i am getting ready to receive a file of size %d\n",size);

    int toRead = size;

    char * filePtr = finalFile;

    while(toRead>0){

    	int n = read(sock_fd, filePtr, toRead);
        if (n<0){
            perror("read");
            exit(0);
        }
        filePtr+=n;
        toRead -=n;

    }

    char * filename = malloc(sizeof(char) * 1024);

    strcpy(filename, "AllFiles-sorted-");
    strcat(filename, finalColumn);
    strcat(filename,".csv");

    FILE *fp = fopen(filename, "w");

    if(fp)
    {
        fputs(finalFile,fp);
    }

    fclose(fp);

    free(finalFile);
    free(filename);






    close(sock_fd);


}
void setPort(char* arg){

	strcpy(port,arg);

}

void setServer(char * arg){

	strcpy(server,arg);

}



int main (int argc, char ** argv){

	char outputDir[256];

	char * inputDirectory = malloc(sizeof(char)*1024);
 	getcwd(inputDirectory, sizeof(char)*1024);


 	setServer(argv[4]);


 	setPort(argv[6]);

 	sendColumn(argv[2]);
 	strcpy(finalColumn,argv[2]);

 	int index;
 	// for(index = 2; index < argc; index+=2){
 	// 	switch(argv[index-1][1]){
 	// 		case 'd':
 	// 			inputDirectory = argv[index];
 	// 		case 'o':
 	// 			outputDir = argv[index];	
 	// 	}
 	// }
 	startDirectory(inputDirectory);

	sendSortRequest();

	return 0;
}