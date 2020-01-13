//
// Created by athena on 23/11/19.
//
#include <iostream>
#ifndef TEMPPROJ_QUERYLIST_H
#define TEMPPROJ_QUERYLIST_H

typedef struct listN{
    std::string query;
    struct listN * next;
}ListNode;

typedef struct listC{
    ListNode * start = nullptr;
    ListNode * end = nullptr;
    int queriesNum = 0;
}List1;
typedef struct IlistN{
    uint64_t value;

    struct IlistN * next;
}IListNode;

typedef struct IlistC{
    IListNode * start = nullptr;
    IListNode * end = nullptr;
    int queriesNum = 0;
}List2;
void AddToList(List1 * list, std::string value );
void DeleteList(List1 * list);

void PrintList(List1 * list);




#endif //TEMPPROJ_QUERYLIST_H
