#ifndef SORTER_H
#define SORTER_H

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
#include <pthread.h>a
#include <sys/syscall.h>

/* defined constants */
#define STRUCT_MEM_SIZE 1000000
#define BUFFER_SIZE 10000

/*created struct to pass arguments into traverse_dir
(thread can only hold one argument, so it will hold this struct of other args)*/
typedef struct _ARG_STRUCT {
    char *dir_base;
    char *place_holder;
    char *sort_col_name;
    char *output_dir_name;
    int output_dir_flag;
    char *dir_name;
    char *file_name;
    char *print_dir;
} arg_struct_t;

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

// mergesort.c function delcarations
void Merge_STR(int low, int mid, int high);

void Merge_int(int low, int mid, int high);

void MergeSort(int low, int high, int n);

// other function declarations
static void *traverse_dir(void *arg_struct);

static void print_output(arg_struct_t *argstruct);

static char *split_string(char *str, char const *delimiters);

static int check_for_quotes(const char *string);

static void remove_quotes(char *str);

static char *add_quotes(char *str);

static void trimstring(char *str);

static int check_commas(char *line, char const delimiters[2], int n);

static void return_Column(char *column_sort, int i);

static void print_movies(movie_t *movie_array, int n_movies, char *sort_column, FILE *f);

static void print_global_movies(char *sort_col_name);

static void check_CSV(const char *dirname, char *dir_base);

static char *remove_extension(const char *filename);

static void
sort_file(const char *dirname, char *dir_base, char *sort_col_name, char *output_dir_name, int output_dir_flag);

static void *found_CSVfile(void *argstruct);

static void *found_directory(void *argstruct);

static int getnumthreads();

static void *traverse_dir(void *argstruct);

/*pthread mutex stuff*/
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/*global array to hold all movies from all files plus a counter*/
movie_t total_movies[100000000];
static int n_total_movies = 0;

/*global variables to keep track of total number of directories and files respectively*/
static int counter_dir = 0;
static int counter_file = 0;

static void print_output(arg_struct_t *argstruct) {

    char prefix[50] = "AllFiles-sorted-";
    char csv_extension[5] = ".csv";
    char *output_filename = malloc(sizeof(char) * 1024);

    if (argstruct->output_dir_flag) {
        // use output_dir_name as output directory
        // deal with the output file
        strcat(argstruct->print_dir, "/");
        strcpy(output_filename, argstruct->output_dir_name);
        strcat(output_filename, prefix);
        strcat(output_filename, argstruct->sort_col_name);
        strcat(output_filename, csv_extension);

        // create a file and print output to it
        FILE *f;
        f = fopen(output_filename, "w");
        print_movies(total_movies, n_total_movies, argstruct->sort_col_name, f);

        // close file and free memory
        fclose(f);
        free((void *) output_filename);
    } else {
        // use dir base as output directory
        // deal with the output file
        char temp[BUFFER_SIZE];

        strcpy(temp, argstruct->print_dir);

        if (strcmp(temp, "./") != 0) {
            strcat(temp, "/");
        }

        strcpy(output_filename, temp);
        strcat(output_filename, prefix);
        strcat(output_filename, argstruct->sort_col_name);
        strcat(output_filename, csv_extension);

        // create a file and print output to it
        FILE *f;
        f = fopen(output_filename, "w");
        print_movies(total_movies, n_total_movies, argstruct->sort_col_name, f);

        // close file and free memory
        fclose(f);
        free((void *) output_filename);
    }

    return;
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

/*changed return column to do this for each column before it goes into the CSV file*/
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

static void print_movies(movie_t *movie_array, int n_movies, char *sort_column, FILE *f) {
    int j = 0;
    char movie_title[BUFFER_SIZE];

    fprintf(f,
            "color,director_name,num_critic_for_reviews,duration,director_facebook_likes,actor_3_facebook_likes,actor_2_name,actor_1_facebook_likes,gross,genres,actor_1_name,movie_title,num_voted_users,cast_total_facebook_likes,actor_3_name,facenumber_in_poster,plot_keywords,movie_imdb_link,num_user_for_reviews,language,country,content_rating,budget,title_year,actor_2_facebook_likes,imdb_score,aspect_ratio,movie_facebook_likes\n");

    while (j < n_movies) {

        if (movie_array[j].has_quotes == 1) {
            movie_array[j].movie_title = add_quotes(movie_array[j].movie_title);
        }

        fprintf(f, "%s,%s,%d,%d,%d,%d,%s,%d,%d,%s,%s,%s,%d,%d,%s,%d,%s,%s,%d,%s,%s,%s,%d,%d,%d,%.2f,%.2f,%d\n",
                movie_array[j].color,
                movie_array[j].director_name,
                movie_array[j].num_critic_for_reviews,
                movie_array[j].duration,
                movie_array[j].director_facebook_likes,
                movie_array[j].actor_3_facebook_likes,
                movie_array[j].actor_2_name,
                movie_array[j].actor_1_facebook_likes,
                movie_array[j].gross,
                movie_array[j].genres,
                movie_array[j].actor_1_name,
                movie_array[j].movie_title,
                movie_array[j].num_voted_users,
                movie_array[j].cast_total_facebook_likes,
                movie_array[j].actor_3_name,
                movie_array[j].facenumber_in_poster,
                movie_array[j].plot_keywords,
                movie_array[j].movie_imdb_link,
                movie_array[j].num_user_for_reviews,
                movie_array[j].language,
                movie_array[j].country,
                movie_array[j].content_rating,
                movie_array[j].budget,
                movie_array[j].title_year,
                movie_array[j].actor_2_facebook_likes,
                movie_array[j].imdb_score,
                movie_array[j].aspect_ratio,
                movie_array[j].movie_facebook_likes
        );
        j++;
    }
}

static void print_global_movies(char *sort_col_name) {

    int j;

    fprintf(stdout,
            "color,director_name,num_critic_for_reviews,duration,director_facebook_likes,actor_3_facebook_likes,actor_2_name,actor_1_facebook_likes,gross,genres,actor_1_name,movie_title,num_voted_users,cast_total_facebook_likes,actor_3_name,facenumber_in_poster,plot_keywords,movie_imdb_link,num_user_for_reviews,language,country,content_rating,budget,title_year,actor_2_facebook_likes,imdb_score,aspect_ratio,movie_facebook_likes\n");

    for (j = 0; j < n_total_movies; j++) {

        if (total_movies[j].has_quotes == 1) {
            total_movies[j].movie_title = add_quotes(total_movies[j].movie_title);
        }

        fprintf(stdout, "%s,%s,%d,%d,%d,%d,%s,%d,%d,%s,%s,%s,%d,%d,%s,%d,%s,%s,%d,%s,%s,%s,%d,%d,%d,%.2f,%.2f,%d\n",
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
}

static void check_CSV(const char *dirname, char *dir_base) {
    //check if proper format
    //reset file to beginning
    FILE *file;
    char pathname[1024];
    char line[BUFFER_SIZE + 1];

    sprintf(pathname, "%s/%s", dir_base, dirname);
    file = fopen(pathname, "r");

    if (!file) {
        printf("file error\n");
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

}

static char *remove_extension(const char *filename) {
    char *str;
    char *lastdot;

    if (filename == NULL) {
        return NULL;
    }

    if ((str = malloc(strlen(filename) + 1)) == NULL) {
        return NULL;
    }

    strcpy(str, filename);

    lastdot = strrchr(str, '.');
    if (lastdot != NULL) {
        *lastdot = '\0';
    }

    return str;
}

static char *shorten_pathname(const char *pathname) {
    char *str;
    char *last_slash;

    if (pathname == NULL) {
        return NULL;
    }

    if ((str = malloc(strlen(pathname) + 1)) == NULL) {
        return NULL;
    }

    strcpy (str, pathname);

    last_slash = strrchr(str, '/');
    if (last_slash != NULL) {
        *last_slash = '\0';
    }

    return str;
}

static void
sort_file(const char *dirname, char *dir_base, char *sort_col_name, char *output_dir_name, int output_dir_flag) {
    //input CSV files in that directory
    //sort each individual file
    //store the sorted files in given output file

    // get movie metadata (input from stdin using cat <filename>)

    FILE *file;
    char pathname[1024];

    sprintf(pathname, "%s/%s", dir_base, dirname);
    file = fopen(pathname, "r");


    if (!file)
    {
        fprintf(stderr, "Input from stdin failed.\n");
        exit(1);
    }

    check_CSV(dirname, dir_base);

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
        return_Column(sort_col_name, n_total_movies);

        n_total_movies++;
    }

    return;
}

static void *found_CSVfile(void *argstruct) {

    arg_struct_t *args = (arg_struct_t *) argstruct;

    pthread_mutex_lock(&mutex);

    char path[1000];
    strcpy(path, args->place_holder);

    pthread_mutex_unlock(&mutex);

    FILE *fp2 = fopen("childpids.txt", "a"); //a means append to file
    fprintf(fp2, "\n%lu", pthread_self());
    fclose(fp2);
    sort_file(args -> file_name, path, args->sort_col_name, args->output_dir_name, args->output_dir_flag);
    pthread_exit(NULL);
    exit(0);
}

static void *found_directory(void *argstruct) {

    arg_struct_t *OLDargs = (arg_struct_t *) argstruct;

    arg_struct_t *args = malloc(sizeof(arg_struct_t) * STRUCT_MEM_SIZE);

    char empty1[BUFFER_SIZE] = "";
    char empty2[BUFFER_SIZE] = "";
    char empty3[BUFFER_SIZE] = "";
    char empty4[BUFFER_SIZE] = "";
    char empty5[BUFFER_SIZE] = "";
    char empty6[BUFFER_SIZE] = "";
    char empty7[BUFFER_SIZE] = "";
    int emptyInt = 0;

    args->dir_base=empty1;
    args->place_holder=empty2;
    args->sort_col_name=empty3;
    args->output_dir_name=empty4;
    args->output_dir_flag=emptyInt;
    args->dir_name=empty5;
    args->file_name=empty6;
    args->print_dir=empty7;

    pthread_mutex_lock(&mutex);

    strcpy(args->dir_base,OLDargs->dir_base);
    strcpy(args->place_holder,OLDargs->place_holder);
    strcpy(args->sort_col_name,OLDargs->sort_col_name);
    strcpy(args->output_dir_name,OLDargs->output_dir_name);
    args->output_dir_flag = OLDargs->output_dir_flag;
    strcpy(args->dir_name,OLDargs->dir_name);
    strcpy(args->file_name,OLDargs->file_name);
    strcpy(args->print_dir,OLDargs->print_dir);

    char path[1000];
    strcpy(path,args->place_holder);

    pthread_mutex_unlock(&mutex);

    strcat(path, "/");
    strcat(path, args->dir_name);

    strcpy(args->place_holder, path);

    FILE *fp2 = fopen("childpids.txt", "a"); //a means append to file
    fprintf(fp2, "\n%lu", pthread_self());
    fclose(fp2);

    traverse_dir(args);
    free(args);
    exit(0);
}

static int getnumthreads() {
    int num = counter_dir + counter_file;
    return num;
}

static void *traverse_dir(void *argstruct) {

    //arg_struct_t *args = malloc(sizeof(arg_struct_t) * STRUCT_MEM_SIZE);

    pthread_mutex_lock(&mutex);

    arg_struct_t *args = (arg_struct_t *) argstruct;

    char dir_base[1000];
    strcpy(dir_base,args->place_holder);
    if (DEBUG) printf("dir base: %s\n",dir_base);
    if (DEBUG) printf("placeholder: %s\n",dir_base);

    pthread_mutex_unlock(&mutex);

    struct dirent *dir;

    DIR *d = opendir(dir_base);
    if (d == NULL) {
        fprintf(stderr, "Error opening directory: '%s'\n", dir_base);
        exit(1);
    }

    while ((dir = readdir(d)) != NULL) {

        if (dir->d_name[0] == '.') {
            continue;
        }
        if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..")) {
            continue;
        }

        char testing_directory[BUFFER_SIZE];
        strcpy(testing_directory, dir->d_name);

        args->dir_name = testing_directory;

        uint8_t type = dir->d_type;

        //if non-directory file
        if (type == DT_REG) {
            //if csv file, then sort
            char *dot = strrchr(dir->d_name, '.');

            if ((dot && !strcmp(dot, ".csv")) && (strstr(dir->d_name, "-sorted-") == NULL)) {
                counter_file++;

                args->file_name = dir->d_name;

                pthread_mutex_lock(&mutex);

                pthread_t thread_file[counter_file];

                if (pthread_create(&thread_file[counter_file], NULL, &found_CSVfile, args)) {
                    printf("error \n");
                    exit(1);
                }

                pthread_mutex_unlock(&mutex);
                //pthread_cond_signal(&cv);

                int status;
                status = pthread_join(thread_file[counter_file], NULL);
                if (status != 0) {
                    printf("can't join!\n");
                }
            }
        }
    }

    MergeSort(0, n_total_movies - 1, n_total_movies);
    closedir(d);
    pthread_exit(NULL);
}

void sort_files() {

    /*
     * This function is to be called when the request is sent from a client to sort all files
     * Process:
     * 1 - Check that there are files, if not then return 0
     *
     * THIS FUNCTION:
     * 2 - Traverse directory that contains all files that have been written to the socket
     *          This may be the current directory? Or it may be a sub-directory?
     * 3 - Call sort for each file, creating a new thread for each (same approach as PA2)
     * 4 - Print output to a file named AllFiles-sorted-XXX.csv as (same approach as PA2)
     *
     * 5 - Initiate sending a response that indicates success / failure?
     * 6 - Send file bytes back to client to be reconstructed as csv
     */



}

#endif