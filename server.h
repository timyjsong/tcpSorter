#ifndef SERVER_HEADER
#define SERVER_HEADER

/* included libraries */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <regex.h>
#include <dirent.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <signal.h>
#include <errno.h>
#include <setjmp.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

/* defined constants */
#define STRUCT_MEM_SIZE 1000000
#define BUFFER_SIZE 10000
#define DEBUG 0

/* struct for each row */
typedef struct _MOVIE {
    /* column headers */
    char *color;
    char *director_name;
    int num_critic_for_reviews;
    int duration;
    int director_facebook_likes;
    int actor_3_facebook_likes;
    char *actor_2_name;
    int actor_1_facebook_likes;
    int gross;
    char *genres;
    char *actor_1_name;
    char *movie_title;
    int num_voted_users;
    int cast_total_facebook_likes;
    char *actor_3_name;
    int facenumber_in_poster;
    char *plot_keywords;
    char *movie_imdb_link;
    int num_user_for_reviews;
    char *language;
    char *country;
    char *content_rating;
    int budget;
    int title_year;
    int actor_2_facebook_likes;
    double imdb_score;
    double aspect_ratio;
    int movie_facebook_likes;
    int sortType_int;
    char *sortType_str;
    int has_apostrophes;
    int has_quotes;
} movie_t;

/*global array to hold all movies from all files plus a counter*/
movie_t total_movies[100000000];
static int n_total_movies = 0;

char column[32];
int fileCounter = 0;

/*to keep track of the # of client requests, 
to act as counter for different sort col names */
int num_clients = -1;

/*array for sort col names, stores the column names
uses num_clients as index*/
char *sort_col_name[1000];

static void *acceptConnection(int client_fd);
static char *split_string(char *str, char const *delimiters);
static void check_CSV(char *fileName);
static int check_for_quotes(const char *string);
static void remove_quotes(char *str);
static char *add_quotes(char *str);
static void trimstring(char *str);
static int check_commas(char *line, char const delimiters[2], int n);
static void return_Column(char *column_sort, int i);
static void sort_file(char *fileName, char* column_sort);
static void *output_directory(int buffer[32], int  client_fd);


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

static void print_global_movies() 
{
    FILE* file_total;
    file_total = fopen("output_total.csv","w");
    fprintf(file_total, "color,director_name,num_critic_for_reviews,duration,director_facebook_likes,actor_3_facebook_likes,actor_2_name,actor_1_facebook_likes,gross,genres,actor_1_name,movie_title,num_voted_users,cast_total_facebook_likes,actor_3_name,facenumber_in_poster,plot_keywords,movie_imdb_link,num_user_for_reviews,language,country,content_rating,budget,title_year,actor_2_facebook_likes,imdb_score,aspect_ratio,movie_facebook_likes\n");

    int j;

    /*fprintf(stdout,
            "color,director_name,num_critic_for_reviews,duration,director_facebook_likes,actor_3_facebook_likes,actor_2_name,actor_1_facebook_likes,gross,genres,actor_1_name,movie_title,num_voted_users,cast_total_facebook_likes,actor_3_name,facenumber_in_poster,plot_keywords,movie_imdb_link,num_user_for_reviews,language,country,content_rating,budget,title_year,actor_2_facebook_likes,imdb_score,aspect_ratio,movie_facebook_likes\n");
	*/
    for (j = 0; j < n_total_movies; j++) {

        if (total_movies[j].has_quotes == 1) {
            total_movies[j].movie_title = add_quotes(total_movies[j].movie_title);
        }

        fprintf(file_total, "%s,%s,%d,%d,%d,%d,%s,%d,%d,%s,%s,%s,%d,%d,%s,%d,%s,%s,%d,%s,%s,%s,%d,%d,%d,%.2f,%.2f,%d\n",
                total_movies[j].color,
                total_movies[j].director_name,
                total_movies[j].num_critic_for_reviews,
                total_movies[j].duration,
                total_movies[j].director_facebook_likes,
                total_movies[j].actor_3_facebook_likes,
                total_movies[j].actor_2_name,
                total_movies[j].actor_1_facebook_likes,
                total_movies[j].gross,
                total_movies[j].genres,
                total_movies[j].actor_1_name,
                total_movies[j].movie_title,
                total_movies[j].num_voted_users,
                total_movies[j].cast_total_facebook_likes,
                total_movies[j].actor_3_name,
                total_movies[j].facenumber_in_poster,
                total_movies[j].plot_keywords,
                total_movies[j].movie_imdb_link,
                total_movies[j].num_user_for_reviews,
                total_movies[j].language,
                total_movies[j].country,
                total_movies[j].content_rating,
                total_movies[j].budget,
                total_movies[j].title_year,
                total_movies[j].actor_2_facebook_likes,
                total_movies[j].imdb_score,
                total_movies[j].aspect_ratio,
                total_movies[j].movie_facebook_likes
        );
    }
    fclose(file_total);
}

static void check_CSV(char *fileName) {
    //check if proper format
    //reset file to beginning
    FILE *file;
    char line[BUFFER_SIZE + 1];

    file = fopen(fileName, "r");

    if (!file) {
        printf("errno: %d\nfile error\n", errno);
        exit(0);
    }

    fgets(line, sizeof(line), file);

    int i = 1;
    int check_num = 0;
    char *p = split_string(line, ",");


    while (p) {

        if (*p) {
            // nothing
        } else {
            p = "";
        }

        i++;

        p = split_string(NULL, ",");
        check_num++;
    }

    if (check_num == 28) {
        // success
    } else {
        fprintf(stdout, "Error! Not enough columns.\n");
        exit(1);
    }
    
    rewind(file);
    fclose(file);
}

static int check_for_quotes(const char *string) {
    char match = '\"';
    int flag = 0;
    int i = 0;
    for (i = 0; i < strlen(string); i++) {
        if (string[i] == match) {
            flag = 1;
            break;
        }
    }
    return flag;
}

static void remove_quotes(char *str) {
    int j = 0;
    int i = 0;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] != '"' && str[i] != '\\') {
            str[j++] = str[i];
        }
    }
    if (j > 0) {
        str[j] = 0;
    }
}

static char *add_quotes(char *str) {

    char *result = malloc(strlen(str) + 3);
    strcpy(result, "");

    strcat(result, "\"");
    strcat(result, str);
    strcat(result, "\"");

    return result;
}

static void trimstring(char *str) {

    int index, i;

    index = 0;
    while (str[index] == ' ' || str[index] == '\t' || str[index] == '\n') {
        index++;
    }

    i = 0;
    while (str[i + index] != '\0') {
        str[i] = str[i + index];
        i++;
    }
    str[i] = '\0';


    i = 0;
    index = -1;
    while (str[i] != '\0') {
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n') {
            index = i;
        }

        i++;
    }

    str[index + 1] = '\0';


    char *pr = str, *pw = str;
    while (*pr) {
        if (isascii(*pr)) {
            *pw = *pr;
            pw++;
        }
        pr++;
    }
    *pw = '\0';
}

static int check_commas(char *line, char const delimiters[2], int n) {
    int i = 1;
    char *p = split_string(line, delimiters);

    while (p) {

        if (*p) {

        } else {
            p = "";
        }

        switch (i) {
            case 1 :
                total_movies[n].color = strdup(p);
                break;
            case 2 :
                total_movies[n].director_name = strdup(p);
                break;
            case 3 :
                total_movies[n].num_critic_for_reviews = atoi(p);
                break;
            case 4 :
                total_movies[n].duration = atoi(p);
                break;
            case 5 :
                total_movies[n].director_facebook_likes = atoi(p);
                break;
            case 6 :
                total_movies[n].actor_3_facebook_likes = atoi(p);
                break;
            case 7 :
                total_movies[n].actor_2_name = strdup(p);
                break;
            case 8 :
                total_movies[n].actor_1_facebook_likes = atoi(p);
                break;
            case 9 :
                total_movies[n].gross = atoi(p);
                break;
            case 10 :
                total_movies[n].genres = strdup(p);
                break;
            case 11 :
                total_movies[n].actor_1_name = strdup(p);
                break;
            case 12 : {
                if (check_for_quotes(p) == 1) {
                    total_movies[n].has_quotes = 1;

                    char *p2 = p;
                    trimstring(p);
                    p = split_string(NULL, delimiters);

                    int size = strlen(p);
                    int size2 = 100 + size;

                    char *str3 = (char *) malloc(size2);
                    strcpy(str3, p2);
                    strcat(str3, ",");
                    strcat(str3, p);

                    while (check_for_quotes(p) != 1) {
                        trimstring(p);
                        p = split_string(NULL, delimiters);
                        strcat(str3, ",");
                        strcat(str3, p);
                    }
                    trimstring(str3);

                    remove_quotes(str3);

                    total_movies[n].movie_title = strdup(str3);
                    free(str3);
                    break;
                } else {
                    trimstring(p);
                    total_movies[n].movie_title = strdup(p);
                    break;
                }
                printf("error in case 12");
                break;


            }
            case 13 :
                total_movies[n].num_voted_users = atoi(p);
                break;
            case 14 :
                total_movies[n].cast_total_facebook_likes = atoi(p);
                break;
            case 15 :
                total_movies[n].actor_3_name = strdup(p);
                break;
            case 16 :
                total_movies[n].facenumber_in_poster = atoi(p);
                break;
            case 17 :
                total_movies[n].plot_keywords = strdup(p);
                break;
            case 18 :
                total_movies[n].movie_imdb_link = strdup(p);
                break;
            case 19 :
                total_movies[n].num_user_for_reviews = atoi(p);
                break;
            case 20 :
                total_movies[n].language = strdup(p);
                break;
            case 21 :
                total_movies[n].country = strdup(p);
                break;
            case 22 :
                total_movies[n].content_rating = strdup(p);
                break;
            case 23 :
                total_movies[n].budget = atoi(p);
                break;
            case 24 :
                total_movies[n].title_year = atoi(p);
                break;
            case 25 :
                total_movies[n].actor_2_facebook_likes = atoi(p);
                break;
            case 26 :
                total_movies[n].imdb_score = atof(p);
                break;
            case 27 :
                total_movies[n].aspect_ratio = atof(p);
                break;
            case 28 :
                total_movies[n].movie_facebook_likes = atoi(p);
                break;
            default:
                break;
        }

        i++;

        p = split_string(NULL, delimiters);

    }
    return (0);
}

static void return_Column(char *column_sort, int i) {
    if (strcmp(column_sort, "color") == 0) {
        total_movies[i].sortType_str = total_movies[i].color;
        total_movies[i].sortType_int = -1;
    } else if (strcmp(column_sort, "director_name") == 0) {
        total_movies[i].sortType_str = total_movies[i].director_name;
        total_movies[i].sortType_int = -1;
    } else if (strcmp(column_sort, "num_critic_for_reviews") == 0) {
        total_movies[i].sortType_int = total_movies[i].num_critic_for_reviews;
    } else if (strcmp(column_sort, "duration") == 0) {
        total_movies[i].sortType_int = total_movies[i].duration;
    } else if (strcmp(column_sort, "director_facebook_likes") == 0) {
        total_movies[i].sortType_int = total_movies[i].director_facebook_likes;
    } else if (strcmp(column_sort, "actor_3_facebook_likes") == 0) {
        total_movies[i].sortType_int = total_movies[i].actor_3_facebook_likes;
    } else if (strcmp(column_sort, "actor_2_name") == 0) {
        total_movies[i].sortType_str = total_movies[i].actor_2_name;
        total_movies[i].sortType_int = -1;
    } else if (strcmp(column_sort, "actor_1_facebook_likes") == 0) {
        total_movies[i].sortType_int = total_movies[i].actor_1_facebook_likes;
    } else if (strcmp(column_sort, "gross") == 0) {
        total_movies[i].sortType_int = total_movies[i].gross;
    } else if (strcmp(column_sort, "genres") == 0) {
        total_movies[i].sortType_str = total_movies[i].genres;
        total_movies[i].sortType_int = -1;
    } else if (strcmp(column_sort, "actor_1_name") == 0) {
        total_movies[i].sortType_str = total_movies[i].actor_1_name;
        total_movies[i].sortType_int = -1;
    } else if (strcmp(column_sort, "movie_title") == 0) {
        total_movies[i].sortType_str = total_movies[i].movie_title;
        total_movies[i].sortType_int = -1;
    } else if (strcmp(column_sort, "num_voted_users") == 0) {
        total_movies[i].sortType_int = total_movies[i].num_voted_users;
    } else if (strcmp(column_sort, "cast_total_facebook_likes") == 0) {
        total_movies[i].sortType_int = total_movies[i].cast_total_facebook_likes;
    } else if (strcmp(column_sort, "actor_3_name") == 0) {
        total_movies[i].sortType_str = total_movies[i].actor_3_name;
        total_movies[i].sortType_int = -1;
    } else if (strcmp(column_sort, "facenumber_in_poster") == 0) {
        total_movies[i].sortType_int = total_movies[i].facenumber_in_poster;
    } else if (strcmp(column_sort, "plot_keywords") == 0) {
        total_movies[i].sortType_str = total_movies[i].plot_keywords;
        total_movies[i].sortType_int = -1;
    } else if (strcmp(column_sort, "movie_imdb_link") == 0) {
        total_movies[i].sortType_str = total_movies[i].movie_imdb_link;
        total_movies[i].sortType_int = -1;
    } else if (strcmp(column_sort, "num_user_for_reviews") == 0) {
        total_movies[i].sortType_int = total_movies[i].num_user_for_reviews;
    } else if (strcmp(column_sort, "language") == 0) {
        total_movies[i].sortType_str = total_movies[i].language;
        total_movies[i].sortType_int = -1;
    } else if (strcmp(column_sort, "country") == 0) {
        total_movies[i].sortType_str = total_movies[i].country;
        total_movies[i].sortType_int = -1;
    } else if (strcmp(column_sort, "content_rating") == 0) {
        total_movies[i].sortType_str = total_movies[i].content_rating;
        total_movies[i].sortType_int = -1;
    } else if (strcmp(column_sort, "budget") == 0) {
        total_movies[i].sortType_int = total_movies[i].budget;
    } else if (strcmp(column_sort, "title_year") == 0) {
        total_movies[i].sortType_int = total_movies[i].title_year;
    } else if (strcmp(column_sort, "actor_2_facebook_likes") == 0) {
        total_movies[i].sortType_int = total_movies[i].actor_2_facebook_likes;
    } else if (strcmp(column_sort, "imdb_score") == 0) {
        total_movies[i].sortType_int = total_movies[i].imdb_score;
    } else if (strcmp(column_sort, "aspect_ratio") == 0) {
        total_movies[i].sortType_int = total_movies[i].aspect_ratio;
    } else if (strcmp(column_sort, "movie_facebook_likes") == 0) {
        total_movies[i].sortType_int = total_movies[i].movie_facebook_likes;
    }
}

static void sort_file(char *fileName, char* column_sort) {
    //check CSV files in that directory
    //store the files in given output file
    // get movie metadata (input from stdin using cat <filename>)

    FILE *file;
    char pathname[1024];

    sprintf(pathname, "%s", fileName);

    file = fopen(pathname, "r");

    if (!file) 
    {
        fprintf(stderr, "Input from stdin failed.\n");
        exit(1);
    }
    
    check_CSV(fileName);

    // define a buffer to be used for fgets
    char line[BUFFER_SIZE + 1];
    // reads in first line of the file (column title line)
    fgets(line, sizeof(line), file);

    // constant for the delimiters
    char const delimiters[2] = ",";

    while (!feof(file)) {
        // get the next line and pass it to the checking function to deal with it
        char *next_line = fgets(line, sizeof(line), file);
        if (next_line == NULL) { break; }

        /*deal with commas and populate global array with movie from this next_line*/
        check_commas(next_line, delimiters, n_total_movies);
        /*determine sort column name, and if string or int*/
        return_Column(column_sort, n_total_movies);

        if (DEBUG) {
            /*
            printf("Added a movie.\n");
            printf("Details:\n");
            printf("----\n");
            printf("total_movies[%d].color: %s\n", n_total_movies, total_movies[n_total_movies].color);
            printf("total_movies[%d].movie_title: %s\n", n_total_movies, total_movies[n_total_movies].movie_title);
            printf("----\n");
            */
        }

        n_total_movies++;
    }
    
    fclose(file);

    return;
}

static void *output_directory(int buffer[32], int  client_fd)
{
	char * wholeFile = malloc(BUFFER_SIZE);
    int size = atoi(buffer);
    int toRead = size;
    char * filePtr = wholeFile;
    while (toRead>0)
    {
        int n = read(client_fd, filePtr, toRead);
        filePtr+=n;
        toRead -=n;
    }
    wholeFile[atoi(buffer)+1] = '\0';  


    char *fileName = malloc(sizeof(char) * 1024);

    char outputpath[20] = "outputdirectory";

    sprintf(fileName, "%s%d", outputpath, num_clients);

    mkdir(fileName, 0777);

    sprintf(fileName, "%s%d/%d", outputpath, num_clients, fileCounter);

    strcat(fileName, ".csv");


    FILE *fp = fopen(fileName, "w");

    if(fp)
    {
        fputs(wholeFile,fp);
    }

    fclose(fp);

    sort_file(fileName, sort_col_name[num_clients]);

    fileCounter++;

    printf("===\n");            
    free(wholeFile);
    return(NULL);
}

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

        output_directory(buffer, client_fd);
        
        return(NULL);

    }

    if(buffer[0]=='c'){


    	/*sorts total movies array*/
    	MergeSort(0, n_total_movies - 1, n_total_movies);
        /*this prints all the movies from array to ONE csv file, saved in current directory*/
    	print_global_movies();

        //now delete the other files
        //send sorted file back to client

        printf("I have received a sort request, this means all the files I need are in my directory, and I must sort and send them back\n");
        return(NULL);
        //exit(0);
    } 
    return(NULL);
}
#endif
