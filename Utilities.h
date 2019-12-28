//
// Created by nicklks on 16/10/19.
//

#ifndef JJ_UTILITIES_H
#define JJ_UTILITIES_H
#include "Types.h"
#include "List.h"

#define Size 256
#define CacheSizeTolerance 8000 // this is the CacheSizeTolerance
#define cacheSize 8000
int hashFunction(uint64_t number, int size);
int numberOfHashTableData(List **HashTable, int size);
void hashTableToTable(List **HashTable, int size, uint64_t *Table[2]);
int hashFunction(uint64_t number, int size);
void printHashTable(List **HashTable, int size);
void printRelation(Relation *rel);
Result *SortMergeJoin(Relation *relR, Relation *relS);
uint64_t ListToTable(List *start, uint64_t **relR, uint64_t **relS);

#endif //JJ_UTILITIES_H
