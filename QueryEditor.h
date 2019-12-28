//
// Created by athena on 8/12/19.
//

#ifndef TEMPPROJ_QUERYEDITOR_H
#define TEMPPROJ_QUERYEDITOR_H
#define QUERYPARTS 3

#include "QueryList.h"
#include "DBRelationsReader.h"
#include "UtilQE.h"
#include <cstring>
void JobExecutor(const std::string& queriesFile, Data * data );
short typeOfPredicate(const std::string& Predicate);
//void ExecuteNumericalValueQuery(std::string Predicate, RelationMD **Bindings, int numOfBindings);
int * getPredicateParts(std::string Predicate);
void getLists(List1 ** batchdebug, int numbatches, const std::string &queriesFile);
void batchExecutor(List1 * batch, Data * data);
#endif //TEMPPROJ_QUERYEDITOR_H
