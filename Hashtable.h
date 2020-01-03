//
// Created by athena on 28/12/19.
//

#ifndef TEMPPROJ_HASHTABLE_H
#define TEMPPROJ_HASHTABLE_H

#include "iostream"
#include <cstdint>
#include "Types.h"

int hashFunction(uint64_t value, uint64_t size);
void insertInList(listNode **start, uint64_t element, uint64_t *pointer);
void printHash(listNode ** hash, int size);
bool pointerExistsInList(listNode *start, uint64_t element, uint64_t* pointer);
bool elementExistsInList(listNode *start, uint64_t element);
void insertInHash(listNode ** hash, uint64_t size, uint64_t element, uint64_t *pointer);
bool insertInResult(listNode **HashTable, uint64_t element, uint64_t *pointer, uint64_t hashTableSize);
void initHash(listNode **hash, uint64_t size);
uint64_t findBucket(uint64_t element, uint64_t size);
void deleteHashTable(listNode **HashTable, uint64_t size);

#endif //TEMPPROJ_HASHTABLE_H
