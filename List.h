//
// Created by nicklks on 23/10/19.
//

#ifndef JJ_LIST_H
#define JJ_LIST_H
#include <cstdint>
#include "Types.h"
#define resultFile "List.txt"

void deleteList(List **start);
void addItem(List *start, uint64_t keyR, uint64_t keyS);
List *addItemInList(List *curr, uint64_t keyR, uint64_t keyS);
void addNode(List **start);
void printList(List *start);
void printListInFile(List *start);
int isNodeFull(List *Node, int Size);


#endif //JJ_LIST_H
