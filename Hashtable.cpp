////
//// Created by athena on 28/12/19.
////
//
//#include "Hashtable.h"
//#include <cstdlib>
//#include <iostream>
//#include "Types.h"
//
//int hashFunction(uint64_t value, uint64_t size){
//    return value%size;
//}
//void insertInList(listNode **start, uint64_t element){
//    if(*start == NULL){
//        (*start) = new listNode;
//        (*start)->number = element;
//        (*start)->next = NULL;
//        return;
//    }
//    listNode *temp = new listNode;
//    temp->number = element;
//    temp->next = (*start);
//    (*start) = temp;
//}
//void printHash(listNode ** hash, int size){
//    listNode * curr;
//    for (int i = 0; i < size; ++i) {
//        curr = hash[i];
//        while (curr!= nullptr){
//            std::cout << curr->number<<' ';
//            curr = curr->next;
//        }
//        std::cout << std::endl;
//    }
//}
//
//void initHash(listNode **hash, uint64_t size){
//    for(uint64_t i = 0; i < size; i++){
//        hash[i] = NULL;
//    }
//}
//
//bool elementExistsInList(listNode *start, uint64_t element){
//    listNode *temp = start;
//    while(temp != NULL){
//        if(temp->number == element)
//            return true;
//        temp = temp->next;
//    }
//    return false;
//}
//
//void insertInHash(listNode ** hash, uint64_t size, uint64_t element){
//    int pos = hashFunction(element, size);
//    if(!elementExistsInList(hash[pos], element))
//        insertInList(&hash[pos], element);
//}
//bool insertInResult(listNode **HashTable, uint64_t element, uint64_t hashTableSize) {
//    insertInHash(HashTable, hashTableSize, element);
//
//}
