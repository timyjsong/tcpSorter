// include server.h
#include "server.h"

void Merge_int(int low, int mid, int high)
{
    int mergedSize = high - low + 1;
    movie_t *temp = (movie_t *)malloc(mergedSize * sizeof(movie_t));
    int merge_p = 0;
    int left_p = low;
    int right_p = mid + 1;

    while (left_p <= mid && right_p <= high)
    {

        if (total_movies[left_p].sortType_int < total_movies[right_p].sortType_int)
        {
            temp[merge_p++] = total_movies[left_p++];
        }
        else
        {
            temp[merge_p++] = total_movies[right_p++];
        }
    }

    while (left_p <= mid)
    {
        temp[merge_p++] = total_movies[left_p++];
    }

    while (right_p <= high)
    {
        temp[merge_p++] = total_movies[right_p++];
    }

    assert(merge_p == mergedSize);


    for (merge_p = 0; merge_p < mergedSize; ++merge_p)
        total_movies[low + merge_p] = temp[merge_p];

    free(temp);
}

void Merge_STR(int low, int mid, int high)
{
    int mergedSize = high - low + 1;
    movie_t *temp = (movie_t *)malloc(mergedSize * sizeof(movie_t));
    int merge_p = 0;
    int left_p = low;
    int right_p = mid + 1;

    while (left_p <= mid && right_p <= high)
    {
        if(total_movies[left_p].sortType_str == NULL || total_movies[right_p].sortType_str == NULL) {
            return;
        }
        
        int ret = strcmp(total_movies[left_p].sortType_str, total_movies[right_p].sortType_str);

        if (ret < 0)
        {
            temp[merge_p++] = total_movies[left_p++];
        }
        else
        {
            temp[merge_p++] = total_movies[right_p++];
        }
    }

    while (left_p <= mid)
    {
        temp[merge_p++] = total_movies[left_p++];
    }

    while (right_p <= high)
    {
        temp[merge_p++] = total_movies[right_p++];
    }

    assert(merge_p == mergedSize);


    for (merge_p = 0; merge_p < mergedSize; ++merge_p)
        total_movies[low + merge_p] = temp[merge_p];

    free(temp); 
}

void MergeSort(int low, int high, int n)
{ 
    if (low < high)
    {
        int mid = (low + high) / 2;

        MergeSort(low, mid, n);
        MergeSort(mid + 1, high, n);


        int j = 0;
        while(j<n){

            if(total_movies[j].sortType_int != -1){
                Merge_int(low, mid, high);
                break;
            }
            else if(strcmp(total_movies[j].sortType_str,"NULL")==0){
                continue;
            }
            else {
                Merge_STR(low, mid, high);
                break;
            }
        }        
    }
}