//
// Created by nicklks on 16/10/19.
//
#include <iostream>
#include "Utilities.h"
#include "QuickSort.h"
#include <fstream>
#include <unistd.h>

using namespace std;
int resLoc = 0;

uint64_t getNbitsAsInt(uint64_t number, int n){
    uint64_t local_mask;
    int shiftFactor = 0;
    switch(n){
        case 1:
            local_mask=0b1111111100000000000000000000000000000000000000000000000000000000;
            shiftFactor = 56;
            break;
        case 2:
            local_mask = 0b11111111000000000000000000000000000000000000000000000000;
            shiftFactor = 48;
            break;
        case 3:
            local_mask = 0b111111110000000000000000000000000000000000000000;
            shiftFactor = 40;
            break;
        case 4:
            local_mask = 0b1111111100000000000000000000000000000000;
            shiftFactor = 32;
            break;
        case 5:
            local_mask = 0b11111111000000000000000000000000;
            shiftFactor = 24;
            break;
        case 6:
            local_mask = 0b111111110000000000000000;
            shiftFactor = 16;
            break;
        case 7:
            local_mask = 0b1111111100000000;
            shiftFactor = 8;
            break;
        case 8:
            local_mask = 0b11111111;
            shiftFactor = 0;
            break;
    }
    uint64_t masked =number & local_mask;
    return (masked) >> shiftFactor;
}

void initHist(int *Hist){
    for(int i = 0; i < 256; i++){
        Hist[i]=0;
    }
}


void printRelation(Relation *rel)
{

    for(int i = 0 ; i < rel->num_tuples; i++)
    {
        cout <<i;
        cout << " KEY: " << rel->tuples[i].key << endl;
        cout<< "PAYLOAD: " << rel->tuples[i].payload << endl;
        cout<< endl;
    }
    return;
}
void printRelationInFile(Relation *rel, string file)
{
    ofstream ix;
    ix.open(file);
    for(int i = 0 ; i < rel->num_tuples; i++)
    {
        ix <<i;
        ix << " KEY: " << rel->tuples[i].key << endl;
        ix << "PAYLOAD: " << rel->tuples[i].payload << endl;
        ix << endl;
    }
    ix.close();
    return;
}

void ParseRelation(int start, int finish, int curr8bit, Relation * pRelation, Relation * nRelation){
    if(curr8bit > resLoc)resLoc=curr8bit;
    int Histogram[Size], Psum[Size];
    uint64_t index;
    Relation * res = nRelation;

    if(curr8bit == 9){
        return ;
    }
    initHist(Histogram);
    //create Histogram
    for(int i = start ; i < finish ; i++){
        index = getNbitsAsInt(pRelation->tuples[i].key, curr8bit);

        Histogram[index] ++;
    }
    //create Psum
    Psum[0] = 0;
    for(int i = 1 ; i < Size; i++){
        Psum[i] = Psum[i-1] + Histogram[i-1];
    }
    //create R'

    int pos;
    for (int i = start; i < finish; ++i) {
        pos = getNbitsAsInt(pRelation->tuples[i].key, curr8bit);

        nRelation->tuples[Psum[pos] + start].key = pRelation->tuples[i].key;
        nRelation->tuples[Psum[pos] + start].payload = pRelation->tuples[i].payload;
        Psum[pos]++;
    }
    //check Psum ~ Hist
    int upper=start, lower=start;
    for(int i = 0 ; i < Size; i++){
        lower = upper;
        upper = upper + Histogram[i];
        if(Histogram[i] > CacheSizeTolerance){
            //printRelation(pRelation);
            //printRelation(nRelation);
            ParseRelation(lower,upper,curr8bit+1,nRelation,pRelation);
            //we have to redo
        }
        else
        {
            quickSort(nRelation->tuples, lower, upper-1);
            for(int i = lower; i < upper; i++)
            {
                pRelation->tuples[i].key = nRelation->tuples[i].key;
                pRelation->tuples[i].payload = nRelation->tuples[i].payload;
            }
        }

    }

}

Relation * findRes(int resPos, Relation * pRelation , Relation * nRelation){
    if(resPos%2 == 1)return nRelation;
    else return pRelation;
}

int notDone(int index1, int size1, int index2, int size2){
    return (index1<size1) && (index2<size2);
}
void getMin(Tuple* a, Tuple * b, int * index1, int * index2, int size1, int size2, Tuple * res){
    if(a->key<=b->key && *index1 <size1){
        *index1 = *index1 +1;
        res->key = a->key;
        res->payload = a->payload;
        return ;
    }else{
        *index2 = *index2 +1;
        res->key = b->key;
        res->payload = b->payload;
        return ;
    }
}

List *JoinKeys(List *curr, uint64_t startR, uint64_t endR, uint64_t startS, uint64_t endS, Relation *relR, Relation *relS)
{
    for(uint64_t i = startR; i <= endR; i++)
    {
        for(uint64_t j = startS; j <= endS; j++)
        {
            curr = addItemInList(curr, relR->tuples[i].payload, relS->tuples[j].payload);
        }
    }
    return curr;
}

Result *SortMergeJoin(Relation *relR, Relation *relS)
{
    //we got two results
    Relation * res1, *res2;

    //First Relation

    //this is the aiding relation for first array initialized
    Relation * nRelation1 = new Relation;
    nRelation1->num_tuples=relR->num_tuples;
    nRelation1->tuples = new Tuple[nRelation1->num_tuples];

    //call for relR, nRelation1
    ParseRelation(0,relR->num_tuples,1,relR,nRelation1);

    //find result between relR, nRelation1
    res1 = findRes(resLoc, relR, nRelation1);
//    res1 = nRelation1;

    //first sorted array is in res 1
//    cout <<"PRINTING FINAL R\n";
//    printRelationInFile(res1, "ix1.txt");

    //Second Relation

    //rewind resLoc to 0
    resLoc = 0;

    //this is the aiding relation for first array initialized
    Relation * nRelation2 = new Relation;
    nRelation2->num_tuples=relS->num_tuples;
    nRelation2->tuples = new Tuple[nRelation2->num_tuples];

    //call for relS, nRelation2
    ParseRelation(0,relS->num_tuples,1,relS,nRelation2);

    //find result between relS, nRelation2
    res2 = findRes(resLoc, relS, nRelation2);
//    res2 = nRelation2;

    //second sorted array is in res 2
//    cout <<"PRINTING FINAL S\n";
//    printRelationInFile(res2, "ix2.txt");
    //printRelation(res2);


    uint64_t indexR1=0, indexR2=0, indexS1=0, indexS2=0;
    uint64_t currVal;
    Tuple a,b;
    List * start=NULL;
    addNode(&start);
    List * curr = start;
    while (notDone(indexR2,res1->num_tuples,indexS2,res2->num_tuples))
    {
        a.key=res1->tuples[indexR2].key;
        a.payload=res1->tuples[indexR2].payload;
        b.key=res2->tuples[indexS2].key;
        b.payload=res2->tuples[indexS2].payload;

        if( a.key < b.key )
        {
            indexR2++;
            indexR1 = indexR2;
        }
        else if( b.key < a.key )
        {
            indexS2++;
            indexS1 = indexS2;
        }
        else {
            currVal = a.key;

            do
            {
                if(a.key == currVal)
                    indexR2++;

                if(b.key == currVal)
                    indexS2++;

                if(indexR2 == res1->num_tuples && indexS2 == res2->num_tuples)
                    break;
                if(indexR2 == res1->num_tuples) {
                    while(res2->tuples[indexS2].key == currVal && indexS2 < res2->num_tuples)
                        indexS2++;
                    break;
                }
                if(indexS2 == res2->num_tuples) {
                    while (res1->tuples[indexR2].key == currVal && indexR2 < res1->num_tuples )
                        indexR2++;
                    break;
                }

//                if(indexR2 >= res1->num_tuples){
//                    indexS2++;
//                    break;
//                }
//                if(indexS2 >= res2->num_tuples){
//                    indexR2++;
//                    break;
//                }

                a.key=res1->tuples[indexR2].key;
                a.payload=res1->tuples[indexR2].payload;
                b.key=res2->tuples[indexS2].key;
                b.payload=res2->tuples[indexS2].payload;

            }while(a.key == currVal || b.key == currVal );

            curr = JoinKeys(curr, indexR1, indexR2-1, indexS1, indexS2-1, res1, res2);
            indexR1 = indexR2;
            indexS1 = indexS2;
        }
    }
    delete nRelation1->tuples;
    delete nRelation1;
    delete nRelation2->tuples;
    delete nRelation2;
    Result *result = new Result;
    result->startOfList = start;
    return result;

}

uint64_t ListToTable(List *start, uint64_t **relR, uint64_t **relS)
{
    uint64_t numberOfTuples = 0;
    uint64_t index = 1;
    List *temp = start;
    while(temp != NULL)
    {
        numberOfTuples += temp->index;

        temp = temp->next;
    }
    (*relR) = new uint64_t[numberOfTuples+1];
    (*relS) = new uint64_t[numberOfTuples+1];
    temp = start;
    while(temp != NULL) {
        for (uint64_t i = 0; i < temp->index; i++) {
            (*relR)[index] = temp->rowIDR[i];
            (*relS)[index++] = temp->rowIDS[i];
        }
        temp = temp->next;
    }
    return numberOfTuples+1;
}