//
// Created by nicklks on 23/10/19.
//
#include "List.h"
#include <cstddef>
#include <iostream>
#include <fstream>

using namespace std;

int isNodeFull(List *Node, int Size)
{
    if(Node->index == Size)
        return 1;
    return 0;
}

void addNode(List **start)
{
    if(*start == NULL) {
        *start = new List;
        (*start)->index=0;
        (*start)->next = NULL;
        return;
    }
    List *temp = *start;
    while(temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = new List;
    temp->next->index=0;
    temp->next->next = NULL;
}

void addItem(List *start, uint64_t rowIDR, uint64_t rowIDS)
{
    if(start == NULL)
        return ;
    List *temp = start;
    while(temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->rowIDR[temp->index] = rowIDR;
    temp->rowIDS[temp->index] = rowIDS;
    temp->index++;
}


List *addItemInList(List *curr, uint64_t rowIDR, uint64_t rowIDS)
{
    if(!isNodeFull(curr, tuplesSize)){
        //insert new frontier
        addItem(curr, rowIDR, rowIDS);
        //increase index
    }
    else{
        //create new node
        addNode(&curr);
        curr = curr->next;
        //reset index to 0
        //addTheNewItem
        addItem(curr, rowIDR, rowIDS);
    }
    return curr;
}

void printList(List *start)
{
    List *temp = start;
    int count=0;

    while(temp!=NULL)
    {
        cout << "------------------------NODE: "<< count++<< "------------------------"<<endl;
        cout << endl;
        for(int i = 0; i < tuplesSize; i++) {
            cout << "INDEX: " << i << endl;
            cout << "ROWID R: " << temp->rowIDR[i] << endl;
            cout << "ROWID S: " << temp->rowIDS[i] << endl;
            cout << endl;
        }
        temp = temp->next;
    }
}

void printListInFile(List *start)
{
    ofstream ix;
    ix.open(resultFile);
    List *temp = start;
    int count=0;
//  uint64_t max = start->tuples[0].key;
    while(temp!=NULL)
    {
        ix << "------------------------NODE: "<< count++<< "------------------------"<<endl;
        ix << endl;
        for(int i = 0; i < tuplesSize; i++) {
//            if(temp->tuples[i].key < max && temp->tuples[i].key != 0)
//                cout << "Not sorted" << endl;
            ix << "INDEX: " << i << endl;
            ix << "ROWID R: " << temp->rowIDR[i] << endl;
            ix << "ROWID S: " << temp->rowIDS[i] << endl;
            ix << endl;
        }
        temp = temp->next;
    }
    ix.close();
    return;
}

void deleteList(List **start)
{
    List *temp = *start;
    while(temp != NULL)
    {
        *start = (*start)->next;
        delete temp;
        temp = *start;
    }
}
