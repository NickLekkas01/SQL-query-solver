//
// Created by nicklks on 23/10/19.
//
#include <iostream>
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

void swap(uint64_t * a, uint64_t * b)
{
    uint64_t t = *a;
    *a = *b;
    *b = t;

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

int partition (uint64_t *arr, int low, int high)
{
    uint64_t pivot = arr[high];    // pivot
    int i = (low - 1);  // Index of smaller element

    for (int j = low; j <= high- 1; j++)
    {
        // If current element is smaller than or
        // equal to pivot
        if (arr[j] <= pivot)
        {
            i++;    // increment index of smaller element
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
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

void quickSort(uint64_t *arr, int low, int high)
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

//int binarySearch(uint64_t *arr, uint64_t l, uint64_t r, uint64_t x, uint64_t* newpos)
//{
//    while( l <= r) {
//        if (r - l == 1 && arr[l] != x && arr[r] != x) {
//            //std::cout << "not found!";
//            *newpos = r;
//            return -1;
//        }
//        if (r == 0 && l == 0) {
//            *newpos = 0;
//            return -1;
//        }
//        uint64_t mid = l + (r - l) / 2;
//
//        // If the element is present at the middle
//        // itself
//        if (arr[mid] == x)
//            return mid;
//
//        // If element is smaller than mid, then
//        // it can only be present in left subarray
//        if (x < arr[mid]){
//            if(mid == 0) {
//                *newpos = 0;
//                return -1;
//            }
//            r = mid - 1;
//        }
//
//        // Else the element can only be present
//        // in right subarray
//        if (x > arr[mid] )
//            l = mid + 1;
//    }
//
//    // We reach here when element is not
//    // present in array
//    *newpos = r;
//    return -1;
//}

int binarySearch(uint64_t *arr, uint64_t l, uint64_t r, uint64_t x, uint64_t* newpos)
{
    if(r-l == 1 && arr[l]!=x && arr[r]!=x){
        *newpos = r;
        return -1;
    }
    if(r == 0 && l == 0){
        *newpos = 0;
        return -1;
    }
    if (r >= l) {
        uint64_t mid = l + (r - l) / 2;

        // If the element is present at the middle
        // itself
        if (arr[mid] == x)
            return mid;

        // If element is smaller than mid, then
        // it can only be present in left subarray
        if (arr[mid] > x){
            if(mid == 0) {
                (*newpos) = 0;
                return -1;
            }
            return binarySearch(arr, l, mid - 1, x, newpos);
        }
        // Else the element can only be present
        // in right subarray
        return binarySearch(arr, mid + 1, r, x, newpos);
    }

    // We reach here when element is not
    // present in array
    *newpos = r;
    return -1;
}