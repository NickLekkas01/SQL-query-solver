//
// Created by athena on 13/12/19.
//

#ifndef TEMPPROJ_UTILQE_H
#define TEMPPROJ_UTILQE_H
#include "QueryEditor.h"
typedef struct IntermediateData{
    uint64_t numOfBindings;
    bool * visited;
    uint64_t ** IMResColumnsForJoin;
    uint64_t ** IMResColumnsForFilters;
}IMData;
bool isVisited(int i, bool *pBoolean);

uint64_t *craftNewResultsFromIMResults(const uint64_t *ExistingIMResults, RelationMD *Binding, int ColumnId, int Op, uint64_t value);

bool isTrue(uint64_t value1, int op, uint64_t value);
bool isVisited(int i, bool *pBoolean) ;
void printStrings(std::string * array , int size);

int getParts(std::string query, std::string * QueryParts);
int getNumOfRelations(const std::string& relations);
RelationMD ** getBindings(std::string relations, Data * data, int * bindingsNum);
int getNumOfPredicates(std::string predicates) ;
short typeOfPredicate(const std::string& Predicate);
std::string *getPredicates(std::string predicates, int * predicatesNum);

uint64_t * getResults(uint64_t *CorrespondingBinding, int RowsNum ,const int PredicateParts[4]);
uint64_t *ExecuteNumericalValueQuery(std::string Predicate, RelationMD **Bindings, int numOfBindings, IMData * data);

uint64_t *craftNewResultsFromIMResults(const uint64_t *ExistingIMResults, RelationMD *Binding, int ColumnId, int Op, uint64_t value);



#include "UtilQE.h"

#endif //TEMPPROJ_UTILQE_H
