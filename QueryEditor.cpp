//
// Created by athena on 8/12/19.
//
#include "Utilities.h"
#include "UtilQE.h"
#include "QueryEditor.h"
//uint64_t getResults(RelationMD *CorrespondingBinding, int PredicateParts[4]);



void getDataFromBindings(RelationMD *Binding, int column, Relation *rel);

void getDataFromJoint(uint64_t *Array, int column, Relation *relation);

void AddToData(IMData *data, uint64_t *RowIDS1, uint64_t *RowIDS2, uint64_t numOfTuples);

int getPleiada(bool *visited, int numOfBindings);

int getFromMap(uint64_t *map, uint64_t bindings, uint64_t RelId);

void copyToBuffer(uint64_t *buffer, uint64_t *Intermediate, uint64_t Row, int numOfCols, int newNumOfCols,
                  uint64_t LastOfBuffer);

void copyToNewIMResults(uint64_t *newResults, uint64_t iterationIndex, const uint64_t *buffer, int index);

void putInBuffer(uint64_t *buffer, uint64_t index, const uint64_t *ImRes, int iterationIndex);

void putInImResults(uint64_t *buffer, uint64_t *Results, uint64_t index, int iterationIndex);

using namespace std;
void initializeIMData(IMData * imData, int numOfBindings){
    imData->numOfBindings = numOfBindings;
    imData->visited = new bool[numOfBindings];
    imData->visitedJoint = new bool[numOfBindings];
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

    imData->IMResColumnsForJoin = nullptr ;
    imData->IMResColumnsForFilters = new uint64_t*[numOfBindings];
    for (uint64_t i = 0; i < imData->numOfBindings; ++i) {

        imData->IMResColumnsForFilters[i] = nullptr;
    }
}

void deleteIntermediateData(IMData * imData){

    delete[] imData->IMResColumnsForJoin;
    delete [] imData->Map;

    for (uint64_t i = 0; i < imData->numOfBindings; ++i) {
        delete[] imData->IMResColumnsForFilters[i];
    }
    delete[] imData->visited;

    delete[] imData->visitedJoint;

}

void QueryExecutor(RelationMD **Bindings, string *Predicates, string *Projections, int numOfBindings, int numOfPredicates, int numOfProjections) {
    // initialize IM results here, send them below.
    IMData data;
    initializeIMData(&data, numOfBindings);
    int * PParts = nullptr;
    uint64_t * temp, *R1, *R2;
    for (int i = 0; i < numOfPredicates; ++i) {
        switch(typeOfPredicate(Predicates[i])){
            case 1: //we have a column value compare to number
                //get binded value(pointer to relation) , column value and operator, execute query
                temp = ExecuteNumericalValueQuery(Predicates[i], Bindings, numOfBindings,&data);
                //add to im results
                break;
            case 2:
                //0->binded R1 1-> r1 column || 2->binded r2 3->binded r3
                PParts = getPredicateParts(Predicates[i]);
                //break apart predicate, if is same r exception
                if(PParts[0] == PParts[2]){
                    HandleSameColumnException(PParts, Bindings[PParts[0]], &data);
                    continue;
                    //is same relation exception
                    // if exists in im results, craft
                    //sending column
                    //send to exception function
                    //add results to
                }
                Relation *rel2 = new Relation;
                Relation *rel1 = new Relation;

                if(data.visitedJoint[PParts[0]]){
                    //get data from join array
                } else if (data.visited[PParts[0]]){
                    //getdatafromfilter
                    rel1->num_tuples = data.IMResColumnsForFilters[PParts[0]][1];
                    rel1->tuples = new Tuple[rel1->num_tuples];
                    getDataFromFilter(data.IMResColumnsForFilters[PParts[0]], PParts[1], Bindings[PParts[0]], rel1);

                }else{
                    rel1->num_tuples = Bindings[PParts[0]]->RowsNum;
                    rel1->tuples = new Tuple[rel1->num_tuples];
                    getDataFromBindings(Bindings[PParts[0]],PParts[1], rel1);
                    //get data from bindings
                }
                if (data.visitedJoint[PParts[2]]){
                    getDataFromJoint(data.IMResColumnsForJoin, PParts[3], rel2);
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
                }

                Result *result = SortMergeJoin(rel1, rel2);
                uint64_t numberOfTuples = ListToTable(result->startOfList, R1, R2);
                R1[0] = PParts[0];
                R2[0] = PParts[2];
                AddToData(&data, R1, R2, numberOfTuples);

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
                delete[] PParts;
                delete [] rel1->tuples;
                delete [] rel2->tuples;
                delete rel1;
                delete rel2;
                delete[] R1;
                delete[] R2;
                break;
        }
    }

}

void AddToData(IMData *data, uint64_t *RowIDS1, uint64_t *RowIDS2, uint64_t numOfTuples) {
    uint64_t * fresh[2];
    fresh[0] = RowIDS1;
    fresh[1]=RowIDS2;
    if(data->IMResColumnsForJoin == nullptr){
        data->visitedJoint[RowIDS1[0]]=true;
        data->visitedJoint[RowIDS2[0]]=true;
        data->Map[0] = RowIDS1[0];
        data->Map[1] = RowIDS2[0];
        data->IMResColumnsForJoin = new uint64_t[(numOfTuples-1)*2];
        uint64_t index = 0;
        for(uint64_t i = 1; i < numOfTuples; i++) {
            data->IMResColumnsForJoin[index] = RowIDS1[i];
            data->IMResColumnsForJoin[index+1] = RowIDS2[i];
            index += 2;
        }
        data->numOfPleiades = (numOfTuples-1);
        return;
    }
    int numOfColsInTuple = getPleiada(data->visitedJoint, data->numOfBindings);
    if(data->visitedJoint[RowIDS1[0]] ^ data->visitedJoint[RowIDS2[0]]){
        int commonColumn, commonOfPair;
        if(!data->visitedJoint[RowIDS1[0]]){
            data->visitedJoint[RowIDS1[0]] = true;
            data->Map[numOfColsInTuple] = RowIDS1[0];
            //commonColumn stoixeio einai ayto poy to rowd[0] tou einai sto map. to map exei se poia 8esh pleiadaw emfanizetai. rara
            commonColumn = getFromMap(data->Map, data->numOfBindings, RowIDS2[0]);
            commonOfPair = 1;

        }
        if(!data->visitedJoint[RowIDS2[0]]){
            data->visitedJoint[RowIDS2[0]] = true;
            data->Map[numOfColsInTuple] = RowIDS2[0];
            commonColumn = getFromMap(data->Map, data->numOfBindings, RowIDS1[0]);
            commonOfPair = 0;
        }
        uint64_t pleiades_new=0;
        int newNumOfColsInTuple = numOfColsInTuple + 1;
        uint64_t *temp = new uint64_t[newNumOfColsInTuple];
        uint64_t *Results = new uint64_t[data->numOfPleiades* (numOfTuples-1) * newNumOfColsInTuple];
        for(uint64_t i = 1 ; i < numOfTuples; i++){
            for(uint64_t j = 0; j < data->numOfPleiades; j++){
                if(fresh[commonOfPair][i] == data->IMResColumnsForJoin[j*numOfColsInTuple + commonColumn]){
                    //copy oloklhrh numOfColsInTuple in ptemp, put fresh[!commonOfPair][i]sto teleytaio stoixeio ths pleiadas_new
                    copyToBuffer(temp, data->IMResColumnsForJoin, j, numOfColsInTuple, newNumOfColsInTuple, fresh[1- commonOfPair][i]);
                    //ftemp = data->IMResColumnsForJoin[j*numOfColsInTuple]
                    copyToNewIMResults(Results, newNumOfColsInTuple, temp, pleiades_new);
                    pleiades_new++;
                    //put in new IMresults
                }
            }
        }
        delete [] temp;
        data->numOfPleiades = pleiades_new;
        delete [] data->IMResColumnsForJoin;
        data->IMResColumnsForJoin = Results;
        return;
    }
    //last case is if they both are in imres
    uint64_t *temp = new uint64_t[getPleiada(data->visited, data->numOfBindings)], pleiades_new =0;
    uint64_t *Results = new uint64_t[data->numOfPleiades* getPleiada(data->visited, data->numOfBindings)];
    int col1 = getFromMap(data->Map, data->numOfBindings, RowIDS1[0]), col2 = getFromMap(data->Map, data->numOfBindings, RowIDS2[0]);
    for(uint64_t i = 1 ; i < numOfTuples; i++){
        for(uint64_t j = 0; j < data->numOfPleiades; j++){
            if(fresh[0][i] == data->IMResColumnsForJoin[j*numOfColsInTuple + col1] && fresh[1][i] == data->IMResColumnsForJoin[j*numOfColsInTuple + col2]){
                putInBuffer(temp, pleiades_new, data->IMResColumnsForJoin, getPleiada(data->visited, data->numOfBindings));
                putInImResults(temp, Results, pleiades_new, getPleiada(data->visited, data->numOfBindings));
                pleiades_new++;
            }
        }
    }
    delete [] temp;
    data->numOfPleiades = pleiades_new;
    delete [] data->IMResColumnsForJoin;
    data->IMResColumnsForJoin = Results;
}

void putInImResults(uint64_t *buffer, uint64_t *Results, uint64_t index, int iterationIndex) {
    for (int i = 0; i < iterationIndex; ++i) {
        Results[index+i] = buffer[i];
    }
}

void putInBuffer(uint64_t *buffer, uint64_t index, const uint64_t *ImRes, int iterationIndex) {
    for (int i = 0; i < iterationIndex ; ++i) {
        buffer[i] = ImRes[index + i];
    }
}

void copyToNewIMResults(uint64_t *newResults, uint64_t iterationIndex, const uint64_t *buffer, int index) {
    for (int i = 0; i < iterationIndex; ++i) {
        newResults[index+i] = buffer[i];
    }
}

void copyToBuffer(uint64_t *buffer, uint64_t *Intermediate, uint64_t Row, int numOfCols, int newNumOfCols,
                  uint64_t LastOfBuffer) {
    for (uint64_t i = 0; i < numOfCols; ++i) {
        buffer[i] = Intermediate[(Row*numOfCols)+i];
    }
    buffer[newNumOfCols-1] = LastOfBuffer;
}

//a b
//c d
//e f
int getFromMap(uint64_t *map, uint64_t numOfBindings, uint64_t RelId) {
    for (uint64_t i = 0; i < numOfBindings; ++i) {
        if(map[i] == RelId){
            return i;
        }
    }
    return -1;
}

int getPleiada(bool *visited, int numOfBindings) {
    int res=0;
    for (int i = 0; i < numOfBindings ; ++i) {
        if(visited[i])res++;
    }
    return res;
}

void getDataFromJoint(uint64_t *JoinArray, int column, Relation *relation) {


}

void getDataFromBindings(RelationMD *Binding, int column, Relation *rel) {
    uint64_t index = 0;
    for (uint64_t i = 0; i < Binding->RowsNum; ++i) {
        rel->tuples[i].payload = i;
        rel->tuples[i].key = Binding->RelationSerialData[column*Binding->RowsNum + i];
    }
}

void getDataFromFilter(uint64_t *Array, int column, RelationMD *Binding, Relation *relation) {
    for(uint64_t i = 0; i < relation->num_tuples; i++)
    {
        relation->tuples[i].payload = Array[i+2];
        relation->tuples[i].key = Binding->RelationSerialData[column*Binding->RowsNum+Array[i+2]];
    }
}

uint64_t * HandleSameColumnException(int *PParts, RelationMD *Binding, IMData *imData) {
    //[relid][num of r]
    uint64_t * temp = imData->IMResColumnsForFilters[PParts[0]];
    uint64_t * tempres;
    uint64_t resNum=0;
    if (imData->IMResColumnsForFilters[PParts[0]] != nullptr){
        tempres = new uint64_t[temp[1]];
        for (uint64_t i = 2; i < temp[1]+2; ++i) {
            if(Binding->RelationSerialData[temp[i]*PParts[2]] == Binding->RelationSerialData[temp[i]*PParts[4]]){
                tempres[i-2]=temp[i];
                resNum++;
            }
        }
        delete [] imData->IMResColumnsForFilters[PParts[0]];
    } else{
        tempres = new uint64_t[Binding->RowsNum];
        for (uint64_t i = 0; i < Binding->RowsNum; ++i) {
            if(Binding->RelationSerialData[temp[i]*PParts[2]] == Binding->RelationSerialData[temp[i]*PParts[4]]){
                tempres[i]=temp[i];
                resNum++;
            }
        }
    }
    uint64_t * rvalue = new uint64_t[resNum+2];
    rvalue[0] = PParts[0];
    rvalue[1] = resNum;
    for (int j = 2; j < resNum+2; ++j) {
        rvalue[j] = tempres[j-2];
    }
    delete [] tempres;
    imData->IMResColumnsForFilters[PParts[0]] = rvalue;
    return rvalue;
}

void batchExecutor(List1 * batch, Data * data){
    ListNode * curr = batch->start;
    string QueryParts[QUERYPARTS];
    RelationMD ** Bindings;
    string * Predicates;
    string * Projections = nullptr;
    int numOfBindings, numOfPredicates, numOfProjections=0;
    while (curr!= nullptr){
        cout <<"Now Processing Query: "<<curr->query<<endl;
        getParts(curr->query, QueryParts);
        Bindings = getBindings(QueryParts[0], data, &numOfBindings); //mia delete xrwstoumenh ana epanalhpsh- delete = clear for next (gia na mh fainontai ta allov del sto loop
        //get2ndpart
        Predicates = getPredicates(QueryParts[1], &numOfPredicates);
        //printStrings(Predicates,getNumOfPredicates(QueryParts[1]));
        //todo get 3rd part - projectionsss
        //execute query
        QueryExecutor(Bindings, Predicates, Projections, numOfBindings, numOfPredicates, numOfProjections);
        curr = curr->next;
        delete [] Bindings;
        delete[] Predicates;
    }
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
            PrintList(&list);
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
