//
// Created by nicklks on 23/10/19.
//

#ifndef JJ_QUICKSORT_H
#define JJ_QUICKSORT_H

#include <cstdint>

#include "Types.h"
size_t binary_insert(const void *base, size_t nel, size_t width, const void *key, int (*cmp)(const void *, const void *));

void swap(uint64_t * a1, uint64_t * b1, uint64_t * a2, uint64_t * b2);
void swap(uint64_t * a, uint64_t * b);
int partition (Tuple *arr, int low, int high);
int partition (uint64_t *arr, int low, int high);
void quickSort(Tuple *arr, int low, int high);
void quickSort(uint64_t *arr, int low, int high);
int binarySearch(uint64_t *arr, uint64_t l, uint64_t r, uint64_t x, uint64_t* newpos);


#endif //JJ_QUICKSORT_H
