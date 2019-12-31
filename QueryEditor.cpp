//
// Created by athena on 8/12/19.
//
#include "Utilities.h"
#include "QueryEditor.h"
//uint64_t getResults(RelationMD *CorrespondingBinding, int PredicateParts[4]);
void printResults(uint64_t *sumOfProjections, int numOfProjections);

using namespace std;
void QueryExecutor(RelationMD **Bindings, string *Predicates, int **Projections, int numOfBindings, int numOfPredicates, int numOfProjections);

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

void QueryExecutor(RelationMD **Bindings, string *Predicates, int **Projections, int numOfBindings, int numOfPredicates, int numOfProjections) {
    // initialize IM results here, send them below.
    IMData data;
    initializeIMData(&data, numOfBindings);
    int * PParts = nullptr;
    uint64_t * temp, *R1, *R2;
    for (int i = 0; i < numOfPredicates; ++i) {
        cout <<"Now processing Predicate "<< Predicates[i]<<endl;
        ofstream fchecker("FilterChecker.txt"), bindcheck1("Bindcheck1.txt"), bindcheck2("Bindcheck2.txt");

        switch(typeOfPredicate(Predicates[i])){
            case 1: //we have a column value compare to number
                //get binded value(pointer to relation) , column value and operator, execute query
                temp = ExecuteNumericalValueQuery(Predicates[i], Bindings, numOfBindings,&data);

                for (int k = 0; k < temp[1]; ++k) {
                    fchecker << temp[k] + 1<<endl;
                }

                //add to im results
                break;
            case 2:
                //0->binded R1 1-> r1 column || 2->binded r2 3->binded r3
                PParts = getPredicateParts(Predicates[i]);
                //break apart predicate, if is same r exception
//                if(Bindings[PParts[0]] == Bindings[PParts[2]]){
                if(Bindings[PParts[0] ] == Bindings[PParts[2]]){
                    HandleSameColumnException(PParts, Bindings[PParts[0]], &data);
                    delete[] PParts;
                    break;
                    //is same relation exception
                    // if exists in im results, craft
                    //sending column
                    //send to exception function
                    //add results to
                }
                Relation *rel1 = new Relation;
                Relation *rel2 = new Relation;

                if(data.visitedJoint[PParts[0]] && data.visitedJoint[PParts[2]]){
                    BothExistInImJoinException(&data, Bindings[PParts[0]], Bindings[PParts[2]], PParts);
                    delete[] PParts;
                    break;
                }
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
                    for (int j = 0; j < rel1->num_tuples; ++j) {
                        bindcheck1 << rel1->tuples[j].key<< " " << rel1->tuples[j].payload << endl;
                    }
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
                    for (int j = 0; j < rel2->num_tuples; ++j) {
                        bindcheck2 << rel2->tuples[j].key<< " " << rel2->tuples[j].payload << endl;
                    }
                }
                ofstream r1txt ("R1.txt");
                ofstream r2txt ("R2.txt");
                for (uint64_t  l = 0; l < rel1->num_tuples; ++l) {
                    r1txt<<rel1->tuples->key;
                    r1txt<<rel1->tuples->payload;
                }
                for (uint64_t  l = 0; l < rel2->num_tuples; ++l) {
                    r2txt<<rel2->tuples->key;
                    r2txt<<rel2->tuples->payload;
                }
                r1txt.close();
                r2txt.close();
                ofstream checker("Checker.txt");
                Result *result = SortMergeJoin(rel1, rel2);
                List *temp = result->startOfList;
                while(temp!= NULL) {
                    for(uint64_t w =0; w < temp->index; w++){
                        checker<<temp->rowIDR[w]<<" ";
                        checker<<temp->rowIDS[w];
                        checker << endl;
                    }
                    temp = temp->next;
                }
                checker.close();
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

    for(int i = 0; i < data.numOfBindings; i++)
    {
        if(!data.visitedJoint[i] && data.visited[i])
        {
            int numOfColsInTuple = getPleiada(data.visitedJoint, data.numOfBindings);
            int newNumOfColsInTuple = numOfColsInTuple + 1;
            uint64_t *Results = new uint64_t[data.numOfPleiades * (data.IMResColumnsForFilters[i][1]) * newNumOfColsInTuple];
            uint64_t *temp = new uint64_t[newNumOfColsInTuple];
            data.visitedJoint[i] = true;
            data.Map[numOfColsInTuple] = i;
            uint64_t pleiades_new = 0;
            for (uint64_t j = 0; j < data.IMResColumnsForFilters[i][1]; j++)
            {
                for(uint64_t k = 0; k < data.numOfPleiades; k++)
                {
                    putInBuffer(temp, k * newNumOfColsInTuple, data.IMResColumnsForJoin,
                                getPleiada(data.visitedJoint, data.numOfBindings));
                    putInImResults(temp, Results, pleiades_new * newNumOfColsInTuple,
                                   getPleiada(data.visitedJoint, data.numOfBindings));
                    pleiades_new++;
                }
            }
            delete[] temp;
            data.numOfPleiades = pleiades_new;
            delete[] data.IMResColumnsForJoin;
            data.IMResColumnsForJoin = Results;
        }
    }

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
        int numOfColsInTuple = getPleiada(data.visitedJoint, data.numOfBindings);
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
    deleteIntermediateData(&data);
}
