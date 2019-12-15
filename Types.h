//
// Created by nicklks on 16/10/19.
//

#ifndef JJ_TYPES_H
#define JJ_TYPES_H
#define tuplesSize ( ( ((1024*1024)-sizeof(uint64_t)-sizeof(struct list *)) / sizeof(uint64_t) ) / 2 )

#include <cstdint>
#include "Types.h"


typedef struct tuple
{
    uint64_t key;
    uint64_t payload;
} Tuple;

typedef struct relation
{
    Tuple  *tuples;
    uint64_t num_tuples;
}Relation;

typedef struct list
{
    uint64_t rowIDR[tuplesSize];
    uint64_t rowIDS[tuplesSize];
    uint64_t index;
    struct list *next;
}List;

/** * Type definition for a relation.  It consists of an array of tuples and a size of the relation. */
typedef struct result
{
    List *startOfList;
} Result;

#endif //JJ_TYPES_H
