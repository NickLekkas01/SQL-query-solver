////
//// Created by athena on 28/12/19.
////
//
#include "Hashtable.h"

int hashFunction(uint64_t value, uint64_t size){
    return value%size;
}
void insertInList(listNode **start, uint64_t element, uint64_t *pointer){
    if(*start == NULL){
        (*start) = new listNode;
        (*start)->number = element;
        (*start)->pointer = pointer;
        (*start)->next = NULL;
        return;
    }
    listNode *temp = new listNode;
    temp->number = element;
    temp->pointer = pointer;
    temp->next = (*start);
    (*start) = temp;
}
void printHash(listNode ** hash, int size){
    listNode * curr;
    for (int i = 0; i < size; ++i) {
        curr = hash[i];
        while (curr!= nullptr){
            std::cout << curr->number<<' ';
            curr = curr->next;
        }
        std::cout << std::endl;
    }
}

void initHash(listNode **hash, uint64_t size){
    for(uint64_t i = 0; i < size; i++){
        hash[i] = NULL;
    }
}

bool pointerExistsInList(listNode *start, uint64_t element, uint64_t* pointer){
    listNode *temp = start;
    while(temp != NULL){
        if(temp->number == element && temp->pointer == pointer)
            return true;
        temp = temp->next;
    }
    return false;
}

bool elementExistsInList(listNode *start, uint64_t element){
    listNode *temp = start;
    while(temp != NULL){
        if(temp->number == element)
            return true;
        temp = temp->next;
    }
    return false;
}

uint64_t findBucket(uint64_t element, uint64_t size){
    uint64_t pos = hashFunction(element, size);
    return pos;
}

void deleteHashTable(listNode **HashTable, uint64_t size)
{
    for(uint64_t i = 0; i < size; i++){
        listnode *temp =HashTable[i];
        while(temp != NULL)
        {
            listnode *deleteNode = temp;
            temp = temp->next;
            delete deleteNode;
        }
    }
}

void insertInHash(listNode ** hash, uint64_t size, uint64_t element, uint64_t *pointer){
    uint64_t pos = hashFunction(element, size);
//    if(!pointerExistsInList(hash[pos], element, pointer))
        insertInList(&hash[pos], element, pointer);
}
bool insertInResult(listNode **HashTable, uint64_t element, uint64_t *pointer, uint64_t hashTableSize) {
    insertInHash(HashTable, hashTableSize, element, pointer);

}
