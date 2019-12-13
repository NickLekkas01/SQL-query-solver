//
// Created by nicklks on 23/10/19.
//

#ifndef JJ_QUICKSORT_H
#define JJ_QUICKSORT_H

#include <cstdint>

#include "Types.h"

void swap(uint64_t * a1, uint64_t * b1, uint64_t * a2, uint64_t * b2);
int partition (Tuple *arr, int low, int high);
void quickSort(Tuple *arr, int low, int high);


#endif //JJ_QUICKSORT_H
