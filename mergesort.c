// include sorter.h
#include "sorter.h"

void Merge_int(movie_t *arr, int low, int mid, int high)
{
    int mergedSize = high - low + 1;
    movie_t *temp = (movie_t *)malloc(mergedSize * sizeof(movie_t));
    int merge_p = 0;
    int left_p = low;
    int right_p = mid + 1;

    while (left_p <= mid && right_p <= high)
    {
        if (arr[left_p].sortType_int < arr[right_p].sortType_int)
        {
            temp[merge_p++] = arr[left_p++];
        }
        else
        {
            temp[merge_p++] = arr[right_p++];
        }
    }

    while (left_p <= mid)
    {
        temp[merge_p++] = arr[left_p++];
    }

    while (right_p <= high)
    {
        temp[merge_p++] = arr[right_p++];
    }

    assert(merge_p == mergedSize);


    for (merge_p = 0; merge_p < mergedSize; ++merge_p)
        arr[low + merge_p] = temp[merge_p];

    free(temp);
}

void Merge_STR(movie_t *arr, int low, int mid, int high)
{
    int mergedSize = high - low + 1;
    movie_t *temp = (movie_t *)malloc(mergedSize * sizeof(movie_t));
    int merge_p = 0;
    int left_p = low;
    int right_p = mid + 1;

    while (left_p <= mid && right_p <= high)
    {
        int ret = strcmp(arr[left_p].sortType_str, arr[right_p].sortType_str);

        if (ret < 0)
        {
            temp[merge_p++] = arr[left_p++];
        }
        else
        {
            temp[merge_p++] = arr[right_p++];
        }
    }

    while (left_p <= mid)
    {
        temp[merge_p++] = arr[left_p++];
    }

    while (right_p <= high)
    {
        temp[merge_p++] = arr[right_p++];
    }

    assert(merge_p == mergedSize);


    for (merge_p = 0; merge_p < mergedSize; ++merge_p)
        arr[low + merge_p] = temp[merge_p];

    free(temp); 
}

void MergeSort(movie_t *arr, int low, int high, int n)
{ 
    if (low < high)
    {
        int mid = (low + high) / 2;

        //printf("low: %d, mid: %d, high: %d\n",low, mid, high);

        MergeSort(arr, low, mid, n);
        MergeSort(arr, mid + 1, high, n);

        //printf("low: %d, mid: %d, high: %d\n",low, mid, high);

        int j = 0;
        while(j<n){
            if(arr[j].sortType_int != -1){
                Merge_int(arr, low, mid, high);
                break;
            }
            else if(strcmp(arr[j].sortType_str,"NULL")==0){
                continue;
            }
            else {
                Merge_STR(arr, low, mid, high);
                break;
            }
            j++;
        }
    }    
}
