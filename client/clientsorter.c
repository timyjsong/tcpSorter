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
#include <errno.h>

#define DEBUG 0
#define STR_LEN 256

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

	if(DEBUG) printf("filename attempting to open: %s\n",filename);

    FILE *file = fopen(filename, "r");

    if(!file){
    	perror(NULL);
    }

    struct stat st;

    stat(filename, &st);

	if(DEBUG) printf("actual size: %d\n", st.st_size);

    int size = st.st_size;
    char  * fileSize = calloc(32,sizeof(char)*32);

    sprintf(fileSize, "%d", size);

    char firstMessage[32] = "b";
    strcat(firstMessage,fileSize);

    write(sock_fd, firstMessage, 32);

    char *buffer = malloc(size);

    if(buffer){
    	fread(buffer,1,size,file);
    }


    if(DEBUG) printf("The size of the buffer i am writing to: %d \n",size);

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
    free(buffer);
    return NULL;
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

    if(DEBUG) printf("The column i am sending is: %s\n",columnToSend);

  

    write(sock_fd, columnToSend, 32);

    close(sock_fd);
}



void *startDirectory(void * path){
	DIR *d;
	struct dirent *dir;

    if(DEBUG) printf("strt dir is starting with path: %s\n",path);

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
							//printf("Creating thread to process: %s\n",ufile);
							pthread_create(&tid,NULL, startFileThreaded, file);
							pthread_join(tid,NULL);

						}
					}

				}
			}


		}
		closedir(d);
	}
	
	//free(path);
	return NULL;

}

void sendSortRequest(int output_dir_flag,char output_dir_name[STR_LEN])
{
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

    if(DEBUG) printf("i am getting ready to receive a file of size %d\n",size);

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

    if(output_dir_flag) 
    {
    	strcpy(filename, output_dir_name);
    	strcat(filename, "/");
    	strcat(filename, "AllFiles-sorted-");
    	strcat(filename, finalColumn);
    	strcat(filename, ".csv");
    	
    	if(DEBUG) printf("DEBUG: output_filename: %s\n", filename);

    	FILE *fp = fopen(filename, "w");

	    if(fp)
	    {
	        fputs(finalFile,fp);
	    }

	    fclose(fp);

    }
    else
    {
    	strcpy(filename, "AllFiles-sorted-");
	    strcat(filename, finalColumn);
	    strcat(filename,".csv");

	    FILE *fp = fopen(filename, "w");

	    if(fp)
	    {
	        fputs(finalFile,fp);
	    }

	    fclose(fp);
    }

    free(finalFile);
    free(filename);

    close(sock_fd);
}

static char *split_string(char *str, char const *delimiters) {

	static char *src = NULL;
	char *p, *ret = 0;

	if (str != NULL) {
		src = str;
	}

	if (src == NULL) {
		return NULL;
	}

	if ((p = strpbrk(src, delimiters)) != NULL) {
		*p = 0;
		ret = src;
		src = ++p;
	} else if (*src) {
		ret = src;
		src = NULL;
	}

	return ret;
}

int main (int argc, char ** argv){

	// read all input arguments first
	int c, input_dir_flag = 0, output_dir_flag = 0, c_args = 0;
	char sort_col_name[STR_LEN], input_dir_name[STR_LEN], output_dir_name[STR_LEN];
	while ((c = getopt(argc, argv, "c:p:h:d:o:")) != -1) {
		switch (c) {
			case 'c':
				c_args = 1;
				char* initial_optarg = optarg;

				char *p1 = split_string(initial_optarg, ",");
				strcpy(sort_col_name, p1);

				while (p1) {

					if (*p1) {
					} else {
						p1 = "";
					}
					p1 = split_string(NULL, ",");
				}

				break;
			case 'h':
				strcpy(server,optarg);
				break;
			case 'p':
				strcpy(port, optarg);
				break;
			case 'd':
				strcpy(input_dir_name, optarg);
				input_dir_flag = 1;

				DIR* dir_directory = opendir(input_dir_name);
				if (dir_directory)
				{
					closedir(dir_directory);
				}
				else if (ENOENT == errno)
				{
					printf("ERROR: given directory does not exist. Using current directory.\n");
					input_dir_flag = 0;
				}
				else
				{
					printf("ERROR: given directory failed to open.\n");
					exit(1);
				}
				break;
			case 'o':
				strcpy(output_dir_name, optarg);
				output_dir_flag = 1;

				DIR* dir_output = opendir(output_dir_name);
				if (dir_output)
				{
					closedir(dir_output);
				}
				else if (ENOENT == errno)
				{
					printf("ERROR: output directory does not exist.\n");
					exit(1);
				}
				else
				{
					printf("ERROR: output directory failed to open.\n");
					exit(1);
				}
				break;
			case ':':
				switch (optopt)
				{
					case 'd':
						printf("ERROR: option -%c with default argument value\n", optopt);
						break;
					default:
						fprintf(stderr, "ERROR: option -%c is missing a required argument\n", optopt);
						exit(1);
				}
				break;
			case '?':
				printf("ERROR: unrecognized option\n");
				exit(1);
		}
	}

	if (!input_dir_flag) {
 		getcwd(input_dir_name, sizeof(char)*STR_LEN);
	}

 	sendColumn(sort_col_name);
 	strcpy(finalColumn,sort_col_name);

 	int index;
 	// for(index = 2; index < argc; index+=2){
 	// 	switch(argv[index-1][1]){
 	// 		case 'd':
 	// 			inputDirectory = argv[index];
 	// 		case 'o':
 	// 			output_dir_name = argv[index];
 	// 	}
 	// }
 	startDirectory(input_dir_name);

	sendSortRequest(output_dir_flag, output_dir_name);

	return 0;
}