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
#include <utility>
#include "Perm.h"
#include "JobScheduler.h"

void printResults(uint64_t *sumOfProjections, int numOfProjections);

using namespace std;

void QueryExecutor(string query, Data *dataExt);
void QueryOptimizer1(string *Predicates, int bindings, int predicates, QueryStats *queryStats);


void QueryOptimizer(string *Predicates, int bindings, int predicates, QueryStats *queryStats);

uint64_t getCost1(const int *predCombo, string *Predicates, int joins, QueryStats statistics);

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

void JobExecutor(const string &queriesFile, Data *data) {
    ifstream workload(queriesFile);

    if(!workload){
        cout << "Couldn't open workload file";

    }
    List1 list;
    string query;
    while (getline(workload, query)){
        //if query is F, send batch to process, reset list
        if(query == "F"){
            //send batch to process
            batchExecutor(&list, data);
            DeleteList(&list);
            list.start= nullptr;
            list.end = nullptr;
            continue;
        }
        //add query to list
        AddToList(&list, query);

    }
    workload.close();

}
class QueryJob : public Job {
    Data * dataExt;
    string Query;
public:
    QueryJob( Data *dataExt, string Query) : Job() {
        this->dataExt = dataExt;
        this->Query = std::move(Query);
    }
    int run() override {
        QueryExecutor( Query, dataExt);
    }
};
#define THREADS
void batchExecutor(List1 * batch, Data * data){
    ListNode * curr = batch->start;
    #ifdef THREADS
    JobScheduler scheduler(6);
    #endif
    int i =0;
    while (curr!= nullptr){
        //numOfProjections=0;
        cout <<"Now Processing Query: "<<curr->query<<endl;

        #ifndef THREADS
        QueryExecutor(curr->query, data);
        #endif
        #ifdef THREADS
        scheduler.schedule(new QueryJob(data, curr->query));
        #endif
        curr = curr->next;
        i++;
    }
    #ifdef THREADS
    scheduler.waitUntilAllJobsHaveFinished();
    #endif
}

//#define DEBUG
void QueryExecutor(string query, Data *dataExt) {
    // initialize IM results here, send them below.
    string QueryParts[QUERYPARTS];
    RelationMD ** Bindings;
    string * Predicates;
    int ** Projections = nullptr;
    int numOfBindings, numOfPredicates, numOfProjections=0;
    getParts(move(query), QueryParts);
    Bindings = getBindings(QueryParts[0], dataExt, &numOfBindings); //mia delete xrwstoumenh ana epanalhpsh- delete = clear for next (gia na mh fainontai ta allov del sto loop
    Predicates = getPredicates(QueryParts[1], &numOfPredicates, Bindings);
    Projections = getProjections(QueryParts[2], Projections, numOfBindings, &numOfProjections);

    IMData data;
    initializeIMData(&data, numOfBindings);
    //initialize query statistics
    QueryStats QStats;
    RelationCS ** statistics = initStats(Bindings, numOfBindings, &QStats);

    int * PParts = nullptr;

    QueryOptimizer(Predicates, numOfBindings, numOfPredicates, &QStats);
    deleteStats(statistics, numOfBindings, QStats);

#ifdef DEBUG
//    deleteStats(statistics, numOfBindings, QStats);
    deleteIntermediateData(&data);
    delete [] Predicates;
    delete [] Bindings;
    for(int i = 0; i < numOfProjections; i++)
        delete[] Projections[i];
    delete[] Projections;
    return;
#endif
    for (int i = 0; i < numOfPredicates; ++i) {
        cout <<"Now processing Predicate "<< Predicates[i]<<endl;
        short switchValue =typeOfPredicate(Predicates[i]);
        switch(switchValue){
            default:
                cout <<"Invalid Predicate\n";
                break;
            case 1: //we have a column value compare to number
                //get binded value(pointer to relation) , column value and operator, execute query

                ExecuteNumericalValueQuery(Predicates[i], Bindings, numOfBindings,&data);

                break;
            case 2:
                //0->binded R1 1-> r1 column || 2->binded r2 3->binded r3
                PParts = getPredicateParts(Predicates[i]);
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
//
                }
                Result *result = SortMergeJoin(rel1, rel2);
                List *temp = result->startOfList;

                uint64_t numberOfTuples = 0;
                while(temp!= nullptr) {
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
                break;

        }
    }


    uint64_t *sumOfProjections = new uint64_t[numOfProjections];
    uint64_t size = data.numOfPleiades;

    for (int j = 0; j < numOfProjections; ++j) {
        sumOfProjections[j] = 0;
        int columnInTuple = getFromMap(data.Map, data.numOfBindings, Projections[j][0]);
        uint64_t indexInBinding;
        if(data.visitedJoint[Projections[j][0]]){
            int numOfColsInTuple = getPleiada(data.visitedJoint, data.numOfBindings);
            for(uint64_t k = 0; k < data.numOfPleiades; ++k) {
                indexInBinding = (data.IMResColumnsForJoin[k*numOfColsInTuple + columnInTuple]);
                uint64_t numberOfTuples = (Bindings[Projections[j][0]]->RowsNum);
                int columnForProjection = Projections[j][1];
                sumOfProjections[j] += Bindings[Projections[j][0]]->RelationSerialData[(numberOfTuples*columnForProjection) + indexInBinding];
            }
        }
        else if(data.visited[Projections[j][0]]) {
            int numOfColsInTuple = getPleiada(data.visited, data.numOfBindings);
            int position;
            for (int i = 0; i < numOfBindings; i++){
                if (data.IMResColumnsForFilters[i] == nullptr) continue;
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
    delete [] Bindings;
    delete[] Predicates;
    for(int i = 0; i < numOfProjections; i++)
        delete[] Projections[i];
    delete[] Projections;
    Projections = nullptr;
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
        rvalue[temp[2]][temp[0]] = true;
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
    auto it = set1.begin();

    while (it!= set1.end()){
        if(adjacency[*it][R])return true;
        it++;
    }
    return false;
}

int * GetBestTree(const set<int> &set1, HashTable *BestTree, int iterator) {
    auto it = set1.begin();
    for (int i = 0; i < iterator; ++i) {
        if (BestTree[i].S == set1 || BestTree[i].S.empty()){
            return BestTree[i].tree;
            cout<<"";
        }
    }
    return nullptr;
}

int cost(int *pInt, string *predicates, int numOfPred, int treeIter, QueryStats *qstats) {
    return 0;
}

uint64_t getCost(const int *tree, int treeLength, string *predicates, int predicatesNum, int i, QueryStats pStatistics) {
    uint64_t cost = 0;
    int * curr;

    for (int j = 0; j < treeLength; ++j) {
        for (int k = i; k < predicatesNum; ++k) {

            curr = getPredicateParts(predicates[k]);

            if((tree[j] == curr[0] || tree[j] == curr[2]) &&(tree[j+1] == curr[0] ||tree[j+1] == curr[2])){
                //found one couple, update stat! heh
                //cout << "Now updating stats of Predicate"<<predicates[k]<<endl;
                updateStats(&pStatistics, predicates[k]);
            }

            delete[] curr;
        }
    }
    //pStatistics.stats[0]->upperA=0;
    return pStatistics.stats[tree[treeLength-1]][0].fA;
}

void
updateHashTable(HashTable *BestTree, int *newTree, int iterator, const set<int> &set, string *Predicates, int start,
                int predNum, QueryStats querystats, int treeLength) {
    for (int i = 0; i < iterator; ++i) {
        if(BestTree[i].S == set || BestTree[i].S.empty()){
            delete [] BestTree[i].tree;
            BestTree[i].tree = newTree;

            if(BestTree[i].S.empty())BestTree[i].S = set;
            BestTree[i].cost = getCost(newTree,treeLength,Predicates,predNum,start,querystats);
            return;
        }
    }
}

void printArray(int *pInt, int l) {
    for (int i = 0; i < l; ++i) {
        cout <<pInt[i]<<" ";
    }
    cout << endl;
}

void printSet(const set<int>& myset) {
    for (int it : myset)
        cout << it<<' ';
    cout <<endl;
}

bool PredNotIncluded(string predicate, string *predicates, int bound) {
    for (int i = 0; i < bound; ++i) {
        if(predicate == predicates[i])return false;
    }
    return true;
}

void QueryOptimizer(string *Predicates, int bindings, int predicates, QueryStats *queryStats) {
    auto * BestTree = new HashTable [(int)(pow(2, bindings) - 1)];
    int comboIterator = (int)(pow(2, bindings) - 1);
    int * relationSet = new int [bindings];
    initSet(relationSet, bindings);
    int k;
    bool ** adjacencyMatrix = initAdjacencyMatrix(bindings, Predicates, predicates);
    int ** setIterator = nullptr;
    for (int j = 0; j < pow(2, bindings) - 1; ++j) {
        BestTree[j].cost = 0;
        BestTree[j].tree = nullptr;
        BestTree[j].S = {};
    }
    //init for filters - questions
    for ( k = 0; k < predicates; ++k) {
        if(typeOfPredicate(Predicates[k]) != 1)break;
        updateStats(queryStats, Predicates[k]);
    }
    //apo k mexri telos exei join pou 8a melethsoume
    for (int i = 0; i < bindings; ++i) {
        BestTree[i].tree = new int[1];
        BestTree[i].tree[0] = i;
        BestTree[i].S.insert(i);
    }
    int * currSet;

    for (int l = 1; l <= bindings; ++l) {

        //get all subsets of
        setIterator = new int*[combinationFormula(bindings, l)];
        printCombination(relationSet, bindings, l, setIterator);
        int *newTreeArray = nullptr, *newTreeDebug;
        uint64_t newCostDebug;
        for (int i = 0; i < combinationFormula(bindings, l); ++i) {

            currSet = setIterator[i];
            #ifdef DEBUG
            cout<<"current Set:\n";
            printArray(currSet, l);
            #endif
            for (int j = 0; j < bindings; ++j) {
                #ifdef DEBUG
                cout << "is " << j <<" in current set?\n\n";

                #endif
                set <int> setter(currSet, currSet + l);

                if(inCurrSet(j, currSet, l)){
#ifdef DEBUG
                    cout << "yes " << j <<" is in current set\n\n";
#endif
                    continue;
                }
                if(!connected(j, adjacencyMatrix, setter, bindings)){
                    continue;
                }

                set<int> St(setter);
                St.insert(j);
                //copy currtree to newTree
                int * currTree = GetBestTree(setter, BestTree, comboIterator);
                if(currTree == nullptr)continue;
                newTreeArray = new int [l+1];
                for (int m = 0; m < l; ++m) {
                    newTreeArray[m] = currTree[m];
                }

                newTreeArray[l] = j;
#ifdef DEBUG
                cout<<"previous tree Set:\n";
                printSet(setter);
                cout<<"current tree Array:\n";
                printArray(newTreeArray, l+1);

#endif
                newTreeDebug =GetBestTree(St, BestTree, comboIterator);
#ifdef DEBUG
                cout << "Set St:";
                printSet(St);
                cout<<"Get Best tree result for Set St:\n";
                printArray(currTree, l);
#endif
                //newCostDebug = getCost(newTreeDebug, l, Predicates, predicates, k, *queryStats);
                if( newTreeDebug == nullptr || (getCost(newTreeDebug, l, Predicates, predicates,k , *queryStats)
                  > getCost(newTreeArray, l, Predicates, predicates,k , *queryStats))){
                    updateHashTable(BestTree, newTreeArray, comboIterator, St, Predicates, k, predicates, *(queryStats),
                                    l);

                }
                else{
                    delete [] newTreeArray;
                    newTreeArray = nullptr;
                }

            }
        }

        for (int i = 0; i < combinationFormula(bindings, l); ++i) {
            delete [] setIterator[i];
        }
        delete [] setIterator;
    }
    set <int> finalSet(relationSet, relationSet + bindings), done = {};
    int * tempPred, pos = 0;
    int * finalTree = GetBestTree(finalSet,BestTree,comboIterator);
    string * tempPredStr = new string[predicates - k];


    for (int n = 0; n < bindings ; ++n) {
        //find predicate
        done.insert(finalTree[n]);
        //for all elements in set, find predicate that connects them to next relation


        auto it = done.begin();

        for (int i = k; i < predicates; ++i) {
            tempPred = getPredicateParts(Predicates[i]);
            if((done.find(tempPred[0])!=done.end() && finalTree[n] == tempPred[2]) || ((done.find(tempPred[2])!=done.end() && finalTree[n] == tempPred[0]) && PredNotIncluded(Predicates[i], tempPredStr, bindings-k))){
                tempPredStr[pos] = Predicates[i];
                pos++;
            }

            delete[] tempPred;

        }
        //while (it!= done.end()){

        //it++;
        //}
    }

    for (int i1 = k; i1 < predicates; ++i1) {
        Predicates[i1] = tempPredStr[i1 -k];
    }

    delete [] tempPredStr;

    //printArray(finalTree, bindings);
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
}
bool isNotSCE(string basicString) {
    int* temp = getPredicateParts(std::move(basicString));
    bool rvalue = (temp[0] != temp[2]);
    delete[]temp;
    return rvalue;
}

void CombinationRepetition(int arr[], int n, int r)
{

    int chosen[r+1];

    CombinationRepetitionUtil(chosen, arr, 0, r, 0, n-1);
}

void QueryOptimizer1(string *Predicates, int bindings, int predicates, QueryStats *queryStats) {
    int i;
    for ( i = 0; i < predicates; ++i) {

        if(typeOfPredicate(Predicates[i])!=FILTER && isNotSCE(Predicates[i])){
            break;
        }
        updateStats(queryStats, Predicates[i]);
    }
    int numOfJoins = bindings - i, ** setIterator = nullptr;
    int * setArray = new int[numOfJoins];
    for (int l = 0; l < numOfJoins; ++l) {
        setArray[l] = i+l;
    }
    setIterator = new int*[permutationFormula(numOfJoins)];

    permutationCaller(setArray, 0, numOfJoins - 1, setIterator);
    uint64_t cost = -1, newCost=0, temp;
    int pos=0;
    for (int k = 0; k < permutationFormula(numOfJoins); ++k) {
        temp = getCost1(setIterator[k], Predicates, numOfJoins, *queryStats);
        if(cost > temp){cost = temp;pos = k;}
    }

    cout << "Best pred seq:";
    string tempString, *tempStringArray = new string[numOfJoins];
    //printArray(setIterator[pos], numOfJoins);



    for (int m = 0; m < numOfJoins; ++m) {
        tempStringArray[m] = Predicates[setIterator[pos][m]];
    }
    for (int m = 0; m < numOfJoins; ++m) {
        Predicates[i+m] = tempStringArray[m];
    }

    delete [] tempStringArray;
    for (int j = 0; j < permutationFormula(numOfJoins); ++j) {
        delete [] setIterator[j];
    }
    delete [] setArray;
    delete [] setIterator;
}

uint64_t getCost1(const int *predCombo, string *Predicates, int joins, QueryStats statistics) {
    for (int i = 0; i < joins; ++i) {
        updateStats(&statistics, Predicates[predCombo[i]]);
    }
    return statistics.stats[predCombo[joins-1]][0].fA;
}



