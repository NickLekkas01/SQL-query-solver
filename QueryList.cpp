//
// Created by athena on 23/11/19.
//
#include "QueryList.h"

#include <utility>
using namespace std;

//add to list (LIST * , string)
//if list->start ==NULL then make a new node, set both start & end to new node, copy value
//else make temp node, set temp.next to NULL, last -> next = temp, last = temp

void AddToList(List * list, string value ){
    ListNode * temp;
    if(list->start == nullptr){
        temp = new ListNode;
        temp->next = nullptr;
        temp ->query = move(value);
        list->start = temp;
        list->end = temp;
        list->queriesNum++;
    }else{
        temp = new ListNode;
        temp->next = nullptr;
        temp -> query = move(value);
        list->end->next=temp;
        list->end = temp;
        list->queriesNum++;
    }
}
//delete list
//temp = start
//while temp->next !=NULL
//temp1= temp
//delete temp1
//temp = temp->next
void DeleteList(List * list){
    ListNode * temp = list->start,
                *temp1;
    while (temp!= nullptr){
        temp1 = temp;
        temp = temp->next;
        delete temp1;

    }
}

void PrintList(List * list){
    ListNode * temp = list->start;
    while (temp!= nullptr){
        cout << temp->query<< endl;
        temp = temp->next;
    }
}