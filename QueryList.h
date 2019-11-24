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
}List;
void AddToList(List * list, std::string value );
void DeleteList(List * list);

void PrintList(List * list);




#endif //TEMPPROJ_QUERYLIST_H
