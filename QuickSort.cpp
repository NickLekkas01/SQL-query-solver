//
// Created by nicklks on 23/10/19.
//
#include "QuickSort.h"

// A utility function to swap two elements
void swap(uint64_t * a1, uint64_t * b1, uint64_t * a2, uint64_t * b2)
{
    uint64_t t1 = *a1;
    *a1 = *b1;
    *b1 = t1;

    uint64_t t2 = *a2;
    *a2 = *b2;
    *b2 = t2;
}

/* This function takes last element as pivot, places
   the pivot element at its correct position in sorted
    array, and places all smaller (smaller than pivot)
   to left of pivot and all greater elements to right
   of pivot */
int partition (Tuple *arr, int low, int high)
{
    uint64_t pivot = arr[high].key;    // pivot
    int i = (low - 1);  // Index of smaller element

    for (int j = low; j <= high- 1; j++)
    {
        // If current element is smaller than or
        // equal to pivot
        if (arr[j].key <= pivot)
        {
            i++;    // increment index of smaller element
            swap(&arr[i].key, &arr[j].key, &arr[i].payload, &arr[j].payload);
        }
    }
    swap(&arr[i + 1].key, &arr[high].key, &arr[i + 1].payload, &arr[high].payload);
    return (i + 1);
}

/* The main function that implements QuickSort
 arr[] --> Array to be sorted,
  low  --> Starting index,
  high  --> Ending index */
void quickSort(Tuple *arr, int low, int high)
{
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
           at right place */
        int pi = partition(arr, low, high);

        // Separately sort elements before
        // partition and after partition
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}
