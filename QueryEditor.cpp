//
// Created by athena on 8/12/19.
//
#define FILTERRESNUMINDEX 1
#define FILTER 1
#define JOIN 2
#include "Utilities.h"
#include "QueryEditor.h"
#include <cmath>
#include <set>
#include "Perm.h"
#define NoCrossProducts 1
//uint64_t getResults(RelationMD *CorrespondingBinding, int PredicateParts[4]);
void printResults(uint64_t *sumOfProjections, int numOfProjections);

using namespace std;

void QueryExecutor(RelationMD **Bindings, string *Predicates, int **Projections, int numOfBindings, int numOfPredicates, int numOfProjections);
void QueryOptimizer1(string *Predicates, int bindings, int predicates, QueryStats *queryStats);

uint64_t maxInt(const uint64_t a, const uint64_t i);

uint64_t minInt(const uint64_t a, const uint64_t i);

void QueryOptimizer(string *predicates, int bindings, int projections, QueryStats *queryStats);

void initializeIMData(IMData * imData, int numOfBindings){
    imData->numOfPleiades = 0;
    imData->numOfBindings = numOfBindings;
    imData->visited = new bool[numOfBindings];
    imData->visitedJoint = new bool[numOfBindings];
    //perror("");
    imData->Map=new uint64_t[numOfBindings];
    for (int i = 0; i < numOfBindings; ++i) {
        imData->visited[i] = false;
    }
    for (int i = 0; i < numOfBindings; ++i) {
        imData->Map[i] = -1;
    }
    for (int i = 0; i < numOfBindings; ++i) {
        imData->visitedJoint[i] = false;
    }

    imData->HashTable = new listNode **[numOfBindings];
    imData->IMResColumnsForJoin = nullptr ;
    imData->IMResColumnsForFilters = new uint64_t*[numOfBindings];
    for (uint64_t i = 0; i < imData->numOfBindings; ++i) {

        imData->IMResColumnsForFilters[i] = nullptr;
    }
}

void deleteIntermediateData(IMData * imData){

    for(int i = 0; i < getPleiada(imData->visitedJoint, imData->numOfBindings); i++)
    {
        deleteHashTable(imData->HashTable[i], (imData->numOfPleiades/3)+1);
        delete[] imData->HashTable[i];
    }
    delete[] imData->HashTable;

    delete[] imData->IMResColumnsForJoin;
    delete [] imData->Map;

    for (uint64_t i = 0; i < imData->numOfBindings; ++i) {
        delete[] imData->IMResColumnsForFilters[i];
    }
    delete[] imData->IMResColumnsForFilters;

    delete[] imData->visited;

    delete[] imData->visitedJoint;

}

void printResults(uint64_t *sumOfProjections, int numOfProjections) {
    cout << "Results:" << endl;
    for(int i = 0; i < numOfProjections; i++){
        cout << "Projection: "<< i <<" Sum: "<<  sumOfProjections[i] << endl;
    }
    cout << endl;
}

void getLists(List1 ** batchdebug, int numbatches, const string &queriesFile){
    ifstream workload(queriesFile);
    if(!workload){
        cout << "Couldn't open workload file";

    }
    int index = 0;
    List1 * list = new List1;
    string query;
    while (getline(workload, query)){
        //cout << query<< endl;
        //if query is F, send batch to process, reset list
        if(query == "F"){
            //send batch to process
            batchdebug[index] = list;
            list= nullptr;
            list = new List1;
            index++;
            continue;
        }
        //add query to list
        AddToList(list, query);
        //PrintList(&list);

    }
    workload.close();
}

void JobExecutor(const string &queriesFile, Data *data) {
    ifstream workload(queriesFile);
    if(!workload){
        cout << "Couldn't open workload file";

    }
    List1 list;
    string query;
    while (getline(workload, query)){
        //cout << query<< endl;
        //if query is F, send batch to process, reset list
        if(query == "F"){
            //send batch to process
            batchExecutor(&list, data);
            //PrintList(&list);
            DeleteList(&list);
            list.start= nullptr;
            list.end = nullptr;
            continue;
        }
        //add query to list
        AddToList(&list, query);
        //PrintList(&list);

    }
    workload.close();

}

void batchExecutor(List1 * batch, Data * data){
    ListNode * curr = batch->start;
    string QueryParts[QUERYPARTS];
    RelationMD ** Bindings;
    string * Predicates;
    int ** Projections = nullptr;
    int numOfBindings, numOfPredicates, numOfProjections=0;
    while (curr!= nullptr){
        //numOfProjections=0;
        cout <<"Now Processing Query: "<<curr->query<<endl;
        getParts(curr->query, QueryParts);
        Bindings = getBindings(QueryParts[0], data, &numOfBindings); //mia delete xrwstoumenh ana epanalhpsh- delete = clear for next (gia na mh fainontai ta allov del sto loop
        //get2ndpart
        Predicates = getPredicates(QueryParts[1], &numOfPredicates, Bindings);
        //todo get 3rd part - projectionsss
        Projections = getProjections(QueryParts[2], Projections, numOfBindings, &numOfProjections);
        //execute query
        QueryExecutor(Bindings, Predicates, Projections, numOfBindings, numOfPredicates, numOfProjections);

        curr = curr->next;
        delete [] Bindings;
        delete[] Predicates;
        for(int i = 0; i < numOfProjections; i++)
            delete[] Projections[i];
        delete[] Projections;
        Projections = nullptr;
    }
}

RelationCS **initStats(RelationMD **bindings, int numOfBindings, QueryStats *QStats) {
    RelationCS ** rvalue = new RelationCS*[numOfBindings];
    QStats->TuplesPerBinding = new uint8_t[numOfBindings];
    for (int i = 0; i < numOfBindings; ++i) {
        rvalue[i] = new RelationCS[bindings[i]->TuplesNum];
        QStats->TuplesPerBinding[i] = bindings[i]->TuplesNum;
        for (int j = 0; j < bindings[i]->TuplesNum; ++j) {
            rvalue[i][j] = bindings[i]->statistics[j];
        }
    }
    QStats->numOfBindings = numOfBindings;
    QStats->stats = rvalue;
    return rvalue;
} // todo transport to util qe

void deleteStats(RelationCS **stats, int numOfBindings, QueryStats Qstats) {
    delete [] Qstats.TuplesPerBinding;
    for (int i = 0; i < numOfBindings; ++i) {
        delete [] stats[i];
    }
    delete [] stats;
}

void updateStats(QueryStats *statistics, const string& Predicate) {
    uint64_t lowerA, upperA;
    uint64_t fA, distinctA;
    short SwitchValue = typeOfPredicate(Predicate);
    int  * PParts;
    uint64_t * PPartsF;
    switch (SwitchValue){
        case FILTER:
            PPartsF = getNumericalValuePredicateParts(Predicate);
            switch (PPartsF[2]){

                case '=': {
                    statistics->stats[PPartsF[0]][PPartsF[1]].upperA = statistics->stats[PPartsF[0]]->lowerA = PPartsF[3]; // la = ua = k
                    fA = statistics->stats[PPartsF[0]][PPartsF[1]].fA;
                    statistics->stats[PPartsF[0]][PPartsF[1]].fA = (statistics->stats[PPartsF[0]]->fA /
                                                                    statistics->stats[PPartsF[0]]->distinctA);
                    uint64_t distinctA = statistics->stats[PPartsF[0]][PPartsF[1]].distinctA;
                    statistics->stats[PPartsF[0]][PPartsF[1]].distinctA = 1;


                    for (int i = 0; i < statistics->TuplesPerBinding[PPartsF[0]]; ++i) { // foreach column in relation
                        if (i == PPartsF[1]) {
                            continue;
                        }
                        statistics->stats[PPartsF[0]][i].distinctA = statistics->stats[PPartsF[0]][i].distinctA * (1 -(pow(1 -
                                                                                                                        statistics->stats[PPartsF[0]][PPartsF[1]].fA /
                                                                                                                        fA,
                                                                                                                        statistics->stats[PPartsF[0]][i].fA /
                                                                                                                        statistics->stats[PPartsF[0]][i].distinctA)));
                        statistics->stats[PPartsF[0]][i].fA = statistics->stats[PPartsF[0]][PPartsF[1]].fA;
                    }
                }

                break;
                case '>':
                    lowerA = statistics->stats[PPartsF[0]][PPartsF[1]].lowerA;
                    fA = statistics->stats[PPartsF[0]][PPartsF[1]].fA;
                    statistics->stats[PPartsF[0]][PPartsF[1]].lowerA = maxInt(statistics->stats[PPartsF[0]][PPartsF[1]].lowerA, PPartsF[3]);
                    statistics->stats[PPartsF[0]][PPartsF[1]].distinctA = ((uint64_t)((statistics->stats[PPartsF[0]][PPartsF[1]].upperA - statistics->stats[PPartsF[0]][PPartsF[1]].lowerA)*statistics->stats[PPartsF[0]][PPartsF[1]].distinctA)/(statistics->stats[PPartsF[0]][PPartsF[1]].upperA-lowerA));
                    statistics->stats[PPartsF[0]][PPartsF[1]].fA = ((statistics->stats[PPartsF[0]][PPartsF[1]].upperA - statistics->stats[PPartsF[0]][PPartsF[1]].lowerA)/ (statistics->stats[PPartsF[0]][PPartsF[1]].upperA - lowerA));
                    for (int i = 0; i < statistics->TuplesPerBinding[PPartsF[0]]; ++i) { // foreach column in relation
                        if (i == PPartsF[1]){
                            continue;
                        }
                        statistics->stats[PPartsF[0]][i].distinctA = statistics->stats[PPartsF[0]][i].distinctA*(1 - ( pow(1 -statistics->stats[PPartsF[0]][PPartsF[1]].fA/fA, statistics->stats[PPartsF[0]][i].fA/statistics->stats[PPartsF[0]][i].distinctA)));
                        statistics->stats[PPartsF[0]][i].fA = statistics->stats[PPartsF[0]][i].fA;
                    }
                    break;
                case '<':
                    upperA = statistics->stats[PPartsF[0]][PPartsF[1]].upperA;
                    fA = statistics->stats[PPartsF[0]][PPartsF[1]].fA;
                    statistics->stats[PPartsF[0]][PPartsF[1]].upperA = minInt(statistics->stats[PPartsF[0]][PPartsF[1]].upperA, PPartsF[3]);
                    statistics->stats[PPartsF[0]][PPartsF[1]].distinctA = ((statistics->stats[PPartsF[0]][PPartsF[1]].upperA - statistics->stats[PPartsF[0]][PPartsF[1]].lowerA)/(upperA-statistics->stats[PPartsF[0]][PPartsF[1]].lowerA));
                    statistics->stats[PPartsF[0]][PPartsF[1]].fA = (statistics->stats[PPartsF[0]][PPartsF[1]].upperA - statistics->stats[PPartsF[0]][PPartsF[1]].lowerA/ (upperA - statistics->stats[PPartsF[0]][PPartsF[1]].lowerA));
                    for (int i = 0; i < statistics->TuplesPerBinding[PPartsF[0]]; ++i) { // foreach column in relation
                        if (i == PPartsF[1]){
                            continue;
                        }
                        statistics->stats[PPartsF[0]][i].distinctA = statistics->stats[PPartsF[0]][i].distinctA*(1 - ( pow(1 -statistics->stats[PPartsF[0]][PPartsF[1]].fA/fA, statistics->stats[PPartsF[0]][i].fA/statistics->stats[PPartsF[0]][i].distinctA)));
                        statistics->stats[PPartsF[0]][i].fA = statistics->stats[PPartsF[0]][i].fA;
                    }
                    break;
            }
            delete [] PPartsF;
            // */
            break;
        case JOIN:
            PParts = getPredicateParts(Predicate);
            //new lower for both relcolumns in join is max of their two old lower
            distinctA = statistics->stats[PParts[0]][PParts[1]].distinctA;
            statistics->stats[PParts[0]][PParts[1]].upperA = statistics->stats[PParts[2]][PParts[3]].upperA = minInt(statistics->stats[PParts[2]][PParts[3]].upperA,statistics->stats[PParts[0]][PParts[1]].upperA);

            //new upper for both relcolumns in join is min of their old two upper
            statistics->stats[PParts[0]][PParts[1]].lowerA = statistics->stats[PParts[2]][PParts[3]].lowerA = maxInt(statistics->stats[PParts[2]][PParts[3]].lowerA,statistics->stats[PParts[0]][PParts[1]].lowerA);
            statistics->stats[PParts[0]][PParts[1]].fA = statistics->stats[PParts[2]][PParts[3]].fA = ((statistics->stats[PParts[0]][PParts[1]].fA * statistics->stats[PParts[2]][PParts[3]].fA )/(statistics->stats[PParts[0]][PParts[1]].upperA - statistics->stats[PParts[0]][PParts[1]].lowerA +1));
            statistics->stats[PParts[0]][PParts[1]].distinctA = statistics->stats[PParts[2]][PParts[3]].distinctA = ((statistics->stats[PParts[0]][PParts[1]].distinctA * statistics->stats[PParts[2]][PParts[3]].distinctA )/(statistics->stats[PParts[0]][PParts[1]].upperA - statistics->stats[PParts[0]][PParts[1]].lowerA+1));

            //for rest of columns in both
            for (int i = 0; i < statistics->TuplesPerBinding[PParts[0]]; ++i) { // foreach column in relation
                if (i == PParts[1]){
                    continue;
                }
                statistics->stats[PParts[0]][i].distinctA = statistics->stats[PParts[0]][i].distinctA*(1 - ( pow(1 -statistics->stats[PParts[0]][PParts[1]].distinctA/distinctA, statistics->stats[PParts[0]][i].fA/statistics->stats[PParts[0]][i].distinctA)));
                statistics->stats[PParts[0]][i].fA = statistics->stats[PParts[0]][PParts[1]].fA;
            }
            distinctA = statistics->stats[PParts[2]][PParts[3]].distinctA;

            //new upper for both relcolumns in join is min of their old two upper

            //for rest of columns in both
            for (int i = 0; i < statistics->TuplesPerBinding[PParts[2]]; ++i) { // foreach column in relation
                if (i == PParts[3]){
                    continue;
                }
                statistics->stats[PParts[2]][i].distinctA = statistics->stats[PParts[2]][i].distinctA*(1 - ( pow(1 -statistics->stats[PParts[2]][PParts[3]].distinctA/distinctA, statistics->stats[PParts[2]][i].fA/statistics->stats[PParts[2]][i].distinctA)));
                statistics->stats[PParts[2]][i].fA = statistics->stats[PParts[2]][PParts[3]].fA;
            }
            delete [] PParts;
            break;
    }
}

uint64_t minInt(const uint64_t a, const uint64_t i) {
    return ((a<i)? a: i);
}

uint64_t maxInt(const uint64_t a, const uint64_t i) {
    return ((a>i)? a: i);
    return 0;
}

void QueryExecutor(RelationMD **Bindings, string *Predicates, int **Projections, int numOfBindings, int numOfPredicates, int numOfProjections) {
    // initialize IM results here, send them below.

    IMData data;
    initializeIMData(&data, numOfBindings);
    //initialize query statistics
    QueryStats QStats;
    RelationCS ** statistics = initStats(Bindings, numOfBindings, &QStats);

    int * PParts = nullptr;
    uint64_t * PPartsF = nullptr;
    uint64_t * temp, *R1, *R2;
    QueryOptimizer1(Predicates, numOfBindings, numOfPredicates, &QStats);
    //delete statistics;

    deleteStats(statistics, numOfBindings, QStats);//here
    deleteIntermediateData(&data);//here
    return;
    for (int i = 0; i < numOfPredicates; ++i) {
        cout <<"Now processing Predicate "<< Predicates[i]<<endl;
        //ofstream fchecker("FilterChecker.txt"), bindcheck1("Bindcheck1.txt"), bindcheck2("Bindcheck2.txt");
        short switchValue =typeOfPredicate(Predicates[i]);
        switch(switchValue){
            case 1: //we have a column value compare to number
                //get binded value(pointer to relation) , column value and operator, execute query

                temp = ExecuteNumericalValueQuery(Predicates[i], Bindings, numOfBindings,&data);
                //PPartsF = getNumericalValuePredicateParts(Predicates[i]);
                //updateStats(&QStats, switchValue, PPartsF, data, temp[1]);

                //delete [] PPartsF;
                //add to im results
                break;
            case 2:
                //0->binded R1 1-> r1 column || 2->binded r2 3->binded r3
                PParts = getPredicateParts(Predicates[i]);
                //break apart predicate, if is same r exception
//                if(Bindings[PParts[0]] == Bindings[PParts[2]]){
                if(PParts[0] == PParts[2]){
                    HandleSameColumnException(PParts, Bindings[PParts[0]], &data);
                    delete[] PParts;
                    break;
                    //is same relation exception
                    // if exists in im results, craft
                    //sending column
                    //send to exception function
                    //add results to
                }
                if(data.visitedJoint[PParts[0]] && data.visitedJoint[PParts[2]]){
                    BothExistInImJoinException(&data, Bindings[PParts[0]], Bindings[PParts[2]], PParts);
                    delete[] PParts;
                    break;
                }

                Relation *rel1 = new Relation;
                Relation *rel2 = new Relation;


                if(data.visitedJoint[PParts[0]]){
                    //get data from join array
                    getDataFromJoint(&data, PParts[0], rel1, PParts[1], Bindings[PParts[0]]);
                } else if (data.visited[PParts[0]]){
                    //getdatafromfilter
                    rel1->num_tuples = data.IMResColumnsForFilters[PParts[0]][1];
                    rel1->tuples = new Tuple[rel1->num_tuples];
                    getDataFromFilter(data.IMResColumnsForFilters[PParts[0]], PParts[1], Bindings[PParts[0]], rel1);

                }else{
                    rel1->num_tuples = Bindings[PParts[0]]->RowsNum;
                    rel1->tuples = new Tuple[rel1->num_tuples];
                    getDataFromBindings(Bindings[PParts[0]],PParts[1], rel1);
//                    for (int j = 0; j < rel1->num_tuples; ++j) {
//                        bindcheck1 << rel1->tuples[j].key<< " " << rel1->tuples[j].payload << endl;
//                    }
                    //get data from bindings
                }
                if (data.visitedJoint[PParts[2]]){

                    getDataFromJoint(&data, PParts[2], rel2, PParts[3], Bindings[PParts[2]]);
                    //get data from join array
                }else if (data.visited[PParts[2]]){
                    //getdatafromfilter
                    rel2->num_tuples = data.IMResColumnsForFilters[PParts[2]][1];
                    rel2->tuples = new Tuple[rel2->num_tuples];
                    getDataFromFilter(data.IMResColumnsForFilters[PParts[2]], PParts[3], Bindings[PParts[2]], rel2);
                }else{

                    //getDatafrom bindings
                    rel2->num_tuples = Bindings[PParts[2]]->RowsNum;
                    rel2->tuples = new Tuple[rel2->num_tuples];
                    getDataFromBindings(Bindings[PParts[2]],PParts[3], rel2);
//                    for (int j = 0; j < rel2->num_tuples; ++j) {
//                        bindcheck2 << rel2->tuples[j].key<< " " << rel2->tuples[j].payload << endl;
//                    }
                }
                //ofstream r1txt ("R1.txt");
                //ofstream r2txt ("R2.txt");
//                for (uint64_t  l = 0; l < rel1->num_tuples; ++l) {
//                    r1txt<<rel1->tuples->key;
//                    r1txt<<rel1->tuples->payload;
//                }
//                for (uint64_t  l = 0; l < rel2->num_tuples; ++l) {
//                    r2txt<<rel2->tuples->key;
//                    r2txt<<rel2->tuples->payload;
//                }
//                r1txt.close();
//                r2txt.close();
                //ofstream checker("Checker.txt");
                Result *result = SortMergeJoin(rel1, rel2);
                List *temp = result->startOfList;
                while(temp!= NULL) {
//                    for(uint64_t w =0; w < temp->index; w++){
//                        checker<<temp->rowIDR[w]<<" ";
//                        checker<<temp->rowIDS[w];
//                        checker << endl;
//                    }
                    temp = temp->next;
                }
                //checker.close();
//                uint64_t numberOfTuples = ListToTable(result->startOfList, &R1, &R2);
//                R1[0] = (uint64_t )PParts[0];
//                R2[0] = (uint64_t )PParts[2];
//
//                for (int j = 1; j < numberOfTuples; ++j) {
//                    checker << R1[j] + 1 << " " << R2[j] + 1<<endl;
//                }
                uint64_t numberOfTuples = 0;
                temp = result->startOfList;
                while(temp!= NULL) {
                    numberOfTuples += temp->index;
                    temp = temp->next;
                }
                AddToData(&data, result->startOfList, numberOfTuples, PParts[0], PParts[2]);

                cout << endl;
                //else
                //do the 4 cases: 1 im results of joins are empty.
                //check if they exist in filters
                //craft the to be sent array
                //else
                //send it from relation (make a copy of the array with RowIds and delete it )
                //case 2
                //if something exists, but is different from both
                //get the other pair of values
                //and cartestiano ginomeno, save in im results, throw the old out!
                //case 3
                //if either exist in im res already, for every line in results, find if in the according column the returned value is there,
                //keep the numOfColsInTuple, save in new im results, delete old im
                //case 4
                // if both exist, for every line in results,
                deleteList(&result->startOfList);
                delete result;
                delete[] PParts;
                delete[] rel1->tuples;
                delete[] rel2->tuples;
                delete rel1;
                delete rel2;
//                delete[] R1;
//                delete[] R2;
                break;
        }
    }


//    for(int i = 0; i < data.numOfBindings; i++)
//    {
//        if(!data.visitedJoint[i] && data.visited[i])
//        {
//            int numOfColsInTuple = getPleiada(data.visitedJoint, data.numOfBindings);
//            int newNumOfColsInTuple = numOfColsInTuple + 1;
//            uint64_t *Results = new uint64_t[data.numOfPleiades * (data.IMResColumnsForFilters[i][1]) * newNumOfColsInTuple];
//            uint64_t *temp = new uint64_t[newNumOfColsInTuple];
//            data.visitedJoint[i] = true;
//            data.Map[numOfColsInTuple] = i;
//            uint64_t pleiades_new = 0;
//            for (uint64_t j = 0; j < data.IMResColumnsForFilters[i][1]; j++)
//            {
//                for(uint64_t k = 0; k < data.numOfPleiades; k++)
//                {
//                    putInBuffer(temp, k * newNumOfColsInTuple, data.IMResColumnsForJoin,
//                                getPleiada(data.visitedJoint, data.numOfBindings));
//                    putInImResults(temp, Results, pleiades_new * newNumOfColsInTuple,
//                                   getPleiada(data.visitedJoint, data.numOfBindings));
//                    pleiades_new++;
//                }
//            }
//            delete[] temp;
//            data.numOfPleiades = pleiades_new;
//            delete[] data.IMResColumnsForJoin;
//            data.IMResColumnsForJoin = Results;
//        }
//    }

    /*Create Results from Projections */
//    uint64_t **ProjectionResults = new uint64_t*[numOfProjections];
    uint64_t *sumOfProjections = new uint64_t[numOfProjections];
    uint64_t size = data.numOfPleiades;
//    for (int j = 0; j < numOfProjections; ++j) {
//        if (data.visited[j] && data.IMResColumnsForFilters[j][1] > size)
//            size = data.IMResColumnsForFilters[j][1];
//    }
    for (int j = 0; j < numOfProjections; ++j) {
        sumOfProjections[j] = 0;
        int columnInTuple = getFromMap(data.Map, data.numOfBindings, Projections[j][0]);
        uint64_t indexInBinding;
//        ProjectionResults[j] = new uint64_t[size];
        if(data.visitedJoint[Projections[j][0]]){
            int numOfColsInTuple = getPleiada(data.visitedJoint, data.numOfBindings);
            for(uint64_t k = 0; k < data.numOfPleiades; ++k) {
                indexInBinding = (data.IMResColumnsForJoin[k*numOfColsInTuple + columnInTuple]);
                uint64_t numberOfTuples = (Bindings[Projections[j][0]]->RowsNum);
                int columnForProjection = Projections[j][1];
//                ProjectionResults[j][k] = Bindings[Projections[j][0]]->RelationSerialData[(numberOfTuples*columnForProjection) + indexInBinding];
//                sumOfProjections[j] += ProjectionResults[j][k];
                sumOfProjections[j] += Bindings[Projections[j][0]]->RelationSerialData[(numberOfTuples*columnForProjection) + indexInBinding];
            }
        }
        else if(data.visited[Projections[j][0]]) {
            int numOfColsInTuple = getPleiada(data.visited, data.numOfBindings);
            int position;
            for (int i = 0; i < numOfBindings; i++){
                if (data.IMResColumnsForFilters[i] == NULL) continue;
                if ((data.IMResColumnsForFilters[i][0]) == Projections[j][0]) {
                    position = i;
                    break;
                }
            }
//            ProjectionResults[j] = new uint64_t[size];
            for(uint64_t i = 2; i < (data.IMResColumnsForFilters[position][1])+2; i++){
                indexInBinding = data.IMResColumnsForFilters[position][i];
                int numberOfTuples = (Bindings[Projections[j][0]]->RowsNum);
                int columnForProjection = Projections[j][1];
//                ProjectionResults[j][i-2] = Bindings[Projections[j][0]]->RelationSerialData[(numberOfTuples*columnForProjection) + indexInBinding];
//                sumOfProjections[j] += ProjectionResults[j][i-2];
                sumOfProjections[j] += Bindings[Projections[j][0]]->RelationSerialData[(numberOfTuples*columnForProjection) + indexInBinding];
            }
        }
    }
    printResults(sumOfProjections, numOfProjections);
    delete[] sumOfProjections;
    deleteIntermediateData(&data);
    deleteStats(statistics, numOfBindings, QStats);

}

typedef struct opt {
    uint64_t cost = 0;
    int * tree = nullptr;
    set<int>  S;
}HashTable;

void initSet(int *pInt, int bindings) {
    for (int i = 0; i < bindings; ++i) {
        pInt[i] = i;
    }
}
int combinationFormula(int n, int r){
    int temp1=1, temp2=1, temp3=1;
    for (int i = 1; i <= n; ++i) {
        temp1*=i;
    }
    for (int i = 1; i <= r; ++i) {
        temp2*=i;
    }
    for (int i = 1; i <= n -r; ++i) {
        temp3*=i;
    }
    return temp1/(temp2*temp3);
}
int permutationFormula(int n){
    int temp1=1;
    for (int i = 1; i <= n; ++i) {
        temp1*=i;
    }

    return temp1;
}
bool **initAdjacencyMatrix(int bindings, string *Predicates, int predNum) {
    bool ** rvalue = new bool*[bindings];
    int * temp;
    for (int i = 0; i < bindings; ++i) {
        rvalue[i] = new bool[bindings];
        for (int j = 0; j < bindings; ++j) {
            rvalue[i][j] = false;
        }
    }
    for (int k = 0; k < predNum; ++k) {
        if(typeOfPredicate(Predicates[k]) == FILTER)continue;
        temp = getPredicateParts(Predicates[k]);
        if(temp[0] == temp[2]){delete []temp;continue;}
        rvalue[temp[0]][temp[2]] = true;
        delete [] temp;
    }
    return rvalue;
}

bool inCurrSet(int j, const int *currSet, int setIter) {
    for (int k = 0; k < setIter; ++k) {
        if(currSet[k] == j){
            return true;}
    }
    return false;
}

void deleteAdjacency(bool **pBoolean, int bindings) {
    for (int i = 0; i < bindings; ++i) {
        delete[] pBoolean[i];
    }
    delete [] pBoolean;
}

bool connected(int R, bool **adjacency, const set<int>& set1, int bindings) {
    set <int>::iterator it = set1.begin();

    while (it!= set1.end()){
        if(adjacency[*it][R])return true;
        it++;
    }
    return false;
}

int * GetBestTree(const set<int> &set1, HashTable *BestTree, int iterator) {
    auto it = set1.begin();
    for (int i = 0; i < iterator; ++i) {
        if (BestTree[i].S == set1){
            return BestTree[i].tree;
            cout<<"";
        }
    }
    return nullptr;
}

int cost(int *pInt, string *predicates, int numOfPred, int treeIter, QueryStats *qstats) {
    return 0;
}

void QueryOptimizer(string *Predicates, int bindings, int predicates, QueryStats *queryStats) {
    HashTable * BestTree = new HashTable [(int)(pow(2, bindings) - 1)];
    int * relationSet = new int [bindings];
    initSet(relationSet, bindings);

    bool ** adjacencyMatrix = initAdjacencyMatrix(bindings, Predicates, predicates);

    int ** setIterator = nullptr;
    for (int j = 0; j < pow(2, bindings) - 1; ++j) {
        BestTree[j].cost = 0;
        BestTree[j].tree = nullptr;
    }
    //init for filters - questions
    for (int k = 0; k < predicates; ++k) {
        if(typeOfPredicate(Predicates[k]) != 1)continue;
        //temp = getNumericalValuePredicateParts(Predicates[k]);
        //updateStats(queryStats, Predicates[k]);
        //delete [] temp;
    }
    //apo k mexri telos exei join pou 8a melethsoume
    for (int i = 0; i < bindings; ++i) {
        BestTree[i].tree = new int[1];
        BestTree[i].tree[0] = i;
        BestTree[i].S.insert(i);
    }
    int * currSet;
    int temp;

    for (int l = 1; l <= bindings; ++l) {
        //get all subsets of
        setIterator = new int*[combinationFormula(bindings, l)];
        printCombination(relationSet, bindings, l, setIterator);
        cout <<endl;
        for (int i = 0; i < combinationFormula(bindings, l); ++i) {

            currSet = setIterator[i];

            for (int j = 0; j < bindings; ++j) {
                set <int> setter(currSet, currSet + l);
                if(inCurrSet(j, currSet, l)){
                    continue;
                }
                if(!connected(j, adjacencyMatrix, setter, bindings && NoCrossProducts)){
                    continue;
                }
                set<int> St(setter);
                St.insert(j);
                int * currTree = GetBestTree(St, BestTree, (int)(pow(2, bindings) - 1));
                if(cost(GetBestTree(St, BestTree, (int) (pow(2, bindings) - 1)), Predicates, predicates, 0, queryStats) > cost(currTree, nullptr, 0, 0,nullptr) || currTree == nullptr ){}
                //if BestTree(St) == NULL || costBestTree(St) > cost(CurrTree) replace

            }
        }

        for (int i = 0; i < combinationFormula(bindings, l); ++i) {
            delete [] setIterator[i];
        }
        delete [] setIterator;
    }
    deleteAdjacency(adjacencyMatrix, bindings);
    for (int m = 0; m < (int) (pow(2, bindings) - 1); ++m) {
        delete [] BestTree[m].tree;
    }
    delete [] BestTree;
    delete [] relationSet;
}
void CombinationRepetitionUtil(int chosen[], int arr[],
                               int index, int r, int start, int end)
{
    // Since index has become r, current combination is
    // ready to be printed, print
    if (index == r)
    {
        for (int i = 0; i < r; i++)
            printf("%d ", arr[chosen[i]]);
        printf("\n");
        return;
    }

    // One by one choose all elements (without considering
    // the fact whether element is already chosen or not)
    // and recur
    for (int i = start; i <= end; i++)
    {
        chosen[index] = i;
        CombinationRepetitionUtil(chosen, arr, index + 1,
                                  r, i, end);
    }
    return;
}
bool isNotSCE(string basicString) {
    int* temp = getPredicateParts(basicString);
    bool rvalue = (temp[0] != temp[2]);
    delete[]temp;
    return rvalue;
}

void CombinationRepetition(int arr[], int n, int r)
{
    // Allocate memory
    int chosen[r+1];

    // Call the recursice function
    CombinationRepetitionUtil(chosen, arr, 0, r, 0, n-1);
}

void QueryOptimizer1(string *Predicates, int bindings, int predicates, QueryStats *queryStats) {
    int i;
    for ( i = 0; i < predicates; ++i) {

        if(typeOfPredicate(Predicates[i])!=FILTER && isNotSCE(Predicates[i])){
            break;
        }
    }
    int numOfJoins = bindings - i, ** setIterator = nullptr;
    int * setArray = new int[numOfJoins];
    for (int l = 0; l < numOfJoins; ++l) {
        setArray[l] = i+l;
    }
    setIterator = new int*[permutationFormula(numOfJoins)];

    permutationCaller(setArray, 0, numOfJoins - 1, setIterator);
    uint64_t cost = -1, newCost=0;
    int pos;
    for (int k = 0; k < permutationFormula(numOfJoins); ++k) {
        //copystats
        for (int j = 0; j < numOfJoins; ++j) {
            cout << Predicates[setIterator[k][j]]<<" ";
            //get cost of this series of predicates
            if(newCost<cost){
                cost = newCost;
                pos = k;
            }
        }
        cout <<endl;
    }
    for (int j = 0; j < permutationFormula(numOfJoins); ++j) {
        delete [] setIterator[j];
    }
    delete [] setArray;
    delete [] setIterator;
}



