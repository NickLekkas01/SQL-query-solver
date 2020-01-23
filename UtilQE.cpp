//
// Created by athena on 13/12/19.
//

//data->filters[][1] = results num of filter.
#include "QueryEditor.h"
#include "UtilQE.h"
#include "QuickSort.h"
#include <cmath>
#define FILTER 1
#define JOIN 2

void initTemp(uint64_t *pInt, uint64_t pleiades);

void printTemp(uint64_t *pInt, uint64_t num);
uint64_t maxInt(uint64_t a, uint64_t i);

uint64_t minInt(uint64_t a, uint64_t i);

bool isTrue(uint64_t value1, int op, uint64_t value) {
    switch (op){
        default:
            std::cout <<"Invalid filter\n";
            break;
        case '=':
            return value1 == value;
        case '>':
            if(value1 > value){
                return true;
            }{ return false;}
        case '<':
            if(value1 > value){
                return true;
            }{ return false;}
    }
}
using namespace std;


bool isVisited(int i, bool *pBoolean) {
    return pBoolean[i];
}
void printStrings(std::string * array , int size){
    for (int i = 0; i <size ; ++i) {
        std::cout << array[i]<<endl;
    }
}

int getParts(string query, string * QueryParts){
    string delimiter = "|",
            token;
    size_t pos = 0, index = 0;
    while ((pos = query.find(delimiter)) != std::string::npos) {
        token = query.substr(0, pos);
        //std::cout << token << std::endl;
        query.erase(0, pos + delimiter.length());
        if(index < QUERYPARTS - 1) {
            QueryParts[index] = token;
            index ++;
        }
        else{
            cout << " Too many arguments in query.\n";
            return 1;
        }

    }
    QueryParts[QUERYPARTS - 1] = query;
    return 0;
}
int getNumOfRelations(const string& relations){
    int iSpaces = 0;

    //Forgot to mention that strlen is incredibly ineffecient... store ahead of time.
    int strSize = relations.length();
    for(unsigned int iLoop = 0; iLoop < strSize; iLoop++ )
        if(relations [iLoop] == ' ' )
            iSpaces++;

    return iSpaces + 1;
}
RelationMD ** getBindings(string relations, Data * data, int * bindingsNum){
    int numOfBindings = getNumOfRelations(relations);
    *bindingsNum = numOfBindings;
    RelationMD ** temp = new RelationMD * [numOfBindings];
    size_t pos = 0, index = 0;
    string delimiter = " ";
    string token;
    while ((pos = relations.find(delimiter)) != std::string::npos) {
        token = relations.substr(0, pos);
        //std::cout << token << std::endl;
        relations.erase(0, pos + delimiter.length());
        //printDefinedStruct(&data->relationsData[stoi(token)]);
        temp[index] = &data->relationsData[stoi(token)];
        index++;
    }
    temp[numOfBindings - 1] = &data->relationsData[stoi(relations)];
    return temp;
}
int getNumOfPredicates(string predicates) {
    int iSpaces = 0;

    //Forgot to mention that strlen is incredibly ineffecient... store ahead of time.
    int strSize = predicates.length();
    for(unsigned int iLoop = 0; iLoop < strSize; iLoop++ )
        if(predicates [iLoop] == '&' )
            iSpaces++;

    return iSpaces + 1;
}
short typeOfPredicate(const string& Predicate) {
    short iSpaces = 0;

    //Forgot to mention that strlen is incredibly ineffecient... store ahead of time.
    int strSize = Predicate.length();
    for(unsigned int iLoop = 0; iLoop < strSize; iLoop++ )
        if(Predicate[iLoop] == '.' )
            iSpaces++;
    return iSpaces;
}
std::string *getPredicates(std::string predicates, int *predicatesNum, RelationMD **Bindings) {
    int numOfPredicates = getNumOfPredicates(predicates);
    *predicatesNum = numOfPredicates;
    string * temp = new string[numOfPredicates];
    size_t pos = 0, index = 0;
    string delimiter = "&";
    string token;
    while ((pos = predicates.find(delimiter)) != std::string::npos) {
        token = predicates.substr(0, pos);
        temp[index] = token; //todo change
        predicates.erase(0, pos + delimiter.length());

        index++;
    }
    temp[numOfPredicates - 1] = predicates;

    string * temp2 = new string[numOfPredicates];
    index = 0;
    bool * visited = new bool[numOfPredicates];
    for (int i = 0; i < numOfPredicates; ++i) {
        visited[i] = false;
    }
    for(int i = 0; i < numOfPredicates; i++)
    {
        if(typeOfPredicate(temp[i]) == 1){
            temp2[index] = temp[i];
            visited[i] = true;
            index++;

        }

    }
    for(int i = 0; i < numOfPredicates; i++)
    {
        if(typeOfPredicate(temp[i]) == 2 && Bindings[temp[i][0] - '0'] != Bindings[temp[i][4] - '0']){
            temp2[index] = temp[i];
            visited[i] = true;
            index++;
        }
    }
    for(int i = 0; i < numOfPredicates; i++)
    {
        if(!visited[i]){
            temp2[index] = temp[i];
            index++;
        }
    }
    delete[] visited;
    delete[] temp;
    return temp2;
}

int **getProjections(string projections, int **Projections, int numOfBindings, int *numOfProjections)
{
    string tempProjections = projections;

    size_t pos = 0;
    string delimiter = " ";
    string token;
    (*numOfProjections) = 0;
    while ((pos = projections.find(delimiter)) != std::string::npos) {
        token = projections.substr(0, pos);
        (*numOfProjections)++;
        projections.erase(0, pos + delimiter.length());

    }
    (*numOfProjections)++;

    pos = 0;
    projections = tempProjections;

    Projections = new int*[(*numOfProjections)];
    for(int i = 0; i < (*numOfProjections); i++)
        Projections[i] = new int[2];


    for(int i = 0; i < (*numOfProjections); i++){
        Projections[i][0] = projections[pos] - '0';
        Projections[i][1] = projections[pos+2] -'0';
        pos+=4;
    }

    return Projections;

}

uint64_t * getResults(uint64_t *CorrespondingBinding, int RowsNum ,const int PredicateParts[4]) {
    uint64_t * temp = new uint64_t[RowsNum];
    uint64_t RowId = 0, resultsNum = 0;
    switch (PredicateParts[2]){
        case '=':

            for (uint64_t i = PredicateParts[1]*RowsNum; i < (PredicateParts[1]+1)*RowsNum ; ++i) {
                if(CorrespondingBinding[i] == PredicateParts[3]){
                    temp[resultsNum] = RowId;
                    resultsNum ++;
                }
                RowId++;
            }
            break;
        case '>':
            for (uint64_t i = PredicateParts[1]*RowsNum; i < (PredicateParts[1]+1)*RowsNum ; ++i) {
                if(CorrespondingBinding[i] > PredicateParts[3]){
                    temp[resultsNum] = RowId;
                    resultsNum ++;
                }
                RowId++;
            }
            break;
        case '<':
            for (uint64_t i = PredicateParts[1]*RowsNum; i < (PredicateParts[1]+1)*RowsNum ; ++i) {
                if(CorrespondingBinding[i] < PredicateParts[3]){
//                    cout << "query is satisfied!\n";
                    temp[resultsNum] = RowId;
                    resultsNum ++;
                }
                RowId++;
            }
            break;
    }
    //allocate return value , 1d array with
    uint64_t * rvalue = new uint64_t[resultsNum +2];
    rvalue[0] = PredicateParts[0];
    rvalue[1] = resultsNum;
    for (size_t i = 0; i < resultsNum; i++) {
        //std::cout << temp[i] << endl;
        rvalue[i+2] = temp[i];
        //copy values to returv value array
    }

    delete [] temp;
    return rvalue;
}

uint64_t * getNumericalValuePredicateParts(string Predicate) {
    uint64_t * rvalue = new uint64_t[4];
    size_t pos = 0;
    string token;
    pos = Predicate.find('.');
    token = Predicate.substr(0, pos);
    rvalue[0] = stoi(token);
    Predicate.erase(0, pos + 1);
    pos = Predicate.find('=');
    if(pos == string::npos){
        pos = Predicate.find('<');
    }
    if(pos == string::npos){
        pos = Predicate.find('>');
    }
    token = Predicate.substr(0, pos);
    rvalue[1] = stoi(token);
    Predicate.erase(0, pos );
    rvalue[2] = Predicate[0];
    Predicate.erase(0, 1 );
    rvalue[3] = stoi(Predicate);

    return rvalue;
}
int * getPredicateParts(string Predicate){
    size_t pos = 0;
    string token;
    pos = Predicate.find('.');
    token = Predicate.substr(0, pos);
    int  *rvalue= new int[4];
    rvalue[0] = stoi(token);
    Predicate.erase(0, pos + 1);
    pos = Predicate.find('=');
    token = Predicate.substr(0, pos);
    rvalue[1] = stoi(token);
    Predicate.erase(0, pos + 1);
    pos = Predicate.find('.');
    token = Predicate.substr(0, pos);
    rvalue[2] = stoi(token);
    Predicate.erase(0, pos + 1);
    rvalue[3] = stoi(Predicate);
    return rvalue;

}
uint64_t *ExecuteNumericalValueQuery(string Predicate, RelationMD **Bindings, int numOfBindings, IMData * data) {

    size_t pos = 0, index = 0;
    string token;
    int PredicatePart[4], iter = Predicate.length();
    pos = Predicate.find('.');
    token = Predicate.substr(0, pos);
    PredicatePart[0] = stoi(token);
    //if the binding is bigger than the bindings num we have been given its invalid(?)
    Predicate.erase(0, pos + 1);

    if((pos = Predicate.find('=')) != string::npos){
        token = Predicate.substr(0, pos);
        PredicatePart[1] = stoi(token);
        //if pp[1] is larger than bindings [pp[0]]-> numtuples is invalid
        PredicatePart[2] = '=';
    }else if((pos = Predicate.find('>')) != string::npos){
        token = Predicate.substr(0, pos);
        PredicatePart[1] = stoi(token);
        PredicatePart[2] = '>';
    }else if((pos = Predicate.find('<')) != string::npos){
        token = Predicate.substr(0, pos);
        PredicatePart[1] = stoi(token);
        PredicatePart[2] = '<';
    }else{
        cout <<"Unknown operator \n";
    }
    Predicate.erase(0, pos +1 );
    //cout << Predicate<<endl;
    PredicatePart[3] = stoi(Predicate);
    if(!isVisited(PredicatePart[0], data->visited)){
        data->visited[PredicatePart[0]] = true;
        uint64_t * res = getResults(Bindings[PredicatePart[0]]->RelationSerialData,Bindings[PredicatePart[0]]->RowsNum, PredicatePart);
        data->IMResColumnsForFilters[PredicatePart[0]] = res;
        res[0] = PredicatePart[0];
        return res;
    }else{
        uint64_t * crafted = craftNewResultsFromIMResults(data->IMResColumnsForFilters[PredicatePart[0]],Bindings[PredicatePart[0]],PredicatePart[1],PredicatePart[2],PredicatePart[3]);
        delete [] data->IMResColumnsForFilters[PredicatePart[0]];
        data->IMResColumnsForFilters[PredicatePart[0]] = crafted;
        return crafted;
    }

}

uint64_t *craftNewResultsFromIMResults(const uint64_t *ExistingIMResults, RelationMD *Binding, int ColumnId, int Op, uint64_t value) {
    //bunch of rowids. [num of results in exsting results 1]
    uint64_t * temp = new uint64_t[ExistingIMResults[1]], resNums =0;
    for (int i = 2; i < ExistingIMResults[1]+2 ; ++i) {
        //if this meets the query,

        if(isTrue(Binding->RelationSerialData[ExistingIMResults[i]+Binding->RowsNum*ColumnId],Op,value)){
            temp[resNums] = ExistingIMResults[i];
            resNums++;
        }
    }
    uint64_t * rvalue = new uint64_t[resNums+2];
    rvalue[0] = ExistingIMResults[0];
    rvalue[1] = resNums;
    for (size_t i = 0; i < resNums; i++) {
        rvalue[i+2] = temp[i];
        //copy values to returv value array
    }
    delete [] temp;
    return rvalue;
    //return temp;
}

void getDataFromJoint(IMData *data, int RelationId, Relation *relation, int column, RelationMD *binding) {
    int pleiada_size = getPleiada(data->visitedJoint, data->numOfBindings);
    int pos = getFromMap(data->Map, data->numOfBindings, RelationId);
    uint64_t resNum = 0;
//    bool temp[binding->RowsNum];
    uint64_t size = (binding->RowsNum/4)+1;
    bitType *temp2 = new bitType[size];

    for (int k = 0; k < size; ++k) {
//        temp[k] = false;
        temp2[k].a = 0;
        temp2[k].b = 0;
        temp2[k].c = 0;
        temp2[k].d = 0;

    }
    //listNode *UniqueHashTable[binding->RowsNum/4];
    //initHash(UniqueHashTable,binding->RowsNum/4);

    for (uint64_t i = 0; i < data->numOfPleiades; ++i) {
        uint64_t value = data->IMResColumnsForJoin[i * pleiada_size + pos];
        if(value%4 == 0){
            if(temp2[value/4].a == 0) resNum++;
            temp2[value/4].a = 1;
        }else if (value%4 == 1){
            if(temp2[value/4].b == 0) resNum++;
            temp2[value/4].b = 1;
        }else if(value %4 == 2){
            if(temp2[value/4].c == 0) resNum++;
            temp2[value/4].c = 1;
        }else if(value%4 == 3){
            if(temp2[value/4].d == 0) resNum++;
            temp2[value/4].d = 1;
        }

//        if(!temp[data->IMResColumnsForJoin[i * pleiada_size + pos]]){
//            resNum++;
//        }
//        temp[data->IMResColumnsForJoin[i * pleiada_size + pos]] = true;
    }
    relation->num_tuples = resNum;
    relation->tuples = new Tuple[resNum];
    uint64_t index =0;
    for (uint64_t j = 0; j < binding->RowsNum; ++j) {
        bool isTrue = false;
        if(j%4 == 0){
            if(temp2[j/4].a) isTrue = true;
        }else if (j%4 == 1){
            if(temp2[j/4].b) isTrue = true;
        }else if(j %4 == 2){
            if(temp2[j/4].c) isTrue = true;
        }else if(j%4 == 3){
            if(temp2[j/4].d) isTrue = true;
        }
        if(isTrue){
            relation->tuples[index].payload = j;
            relation->tuples[index].key = binding->RelationSerialData[column*binding->RowsNum + j];
            index++;
        }

        //        if(temp[j]){
//            relation->tuples[index].payload = j;
//            relation->tuples[index].key = binding->RelationSerialData[column*binding->RowsNum + j];
//            index++;
//        }
    }
    delete [] temp2;

}


void combinationUtil(int *arr, int n, int r, int index, int *data, int i, int **setIter, int *indexOfOuter) {
    // Current cobination is ready, print it
    if (index == r) {
        setIter[*indexOfOuter] = new int[r];
        for (int j = 0; j < r; j++){
            setIter[*indexOfOuter][j] = data[j];
            //printf("%d ", data[j]);
        }
        *indexOfOuter = (*indexOfOuter) + 1;
        //printf("\n");
        return;
    }

    // When no more elements are there to put in data[]
    if (i >= n)
        return;

    // current is included, put next at next location
    data[index] = arr[i];
    combinationUtil(arr, n, r, index + 1, data, i + 1, setIter, indexOfOuter);

    // current is excluded, replace it with next
    // (Note that i+1 is passed, but index is not
    // changed)
    combinationUtil(arr, n, r, index, data, i + 1, setIter, indexOfOuter);
}

void printCombination(int arr[], int n, int r, int **setIter) {
    // A temporary array to store all combination
    // one by one
    int data[r];
    int indexOfOuter = 0;
    // Print all combination using temprary array 'data[]'
    combinationUtil(arr, n, r, 0, data, 0, setIter, &indexOfOuter );
}


void insertInNewPos(uint64_t *pInt, uint64_t element, uint64_t pos, uint64_t iterationIndex) {
    for (uint64_t i = iterationIndex +1; i > pos; i--)
        pInt[i] = pInt[i - 1];

    // insert x at pos
    pInt[pos ] = element ;
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
void copyToBuffer(uint64_t *buffer, uint64_t *Intermediate, uint64_t Row, int numOfCols, int newNumOfCols,
                  uint64_t LastOfBuffer) {
    for (uint64_t i = 0; i < numOfCols; ++i) {
        buffer[i] = Intermediate[(Row*numOfCols)+i];
    }
    buffer[newNumOfCols-1] = LastOfBuffer;
}

void copyToBuffer(uint64_t *buffer, uint64_t *Intermediate, uint64_t numOfCols, uint64_t lastOfBuffer) {
    for (uint64_t i = 0; i < numOfCols; ++i) {
        buffer[i] = Intermediate[i];
    }
    buffer[numOfCols] = lastOfBuffer;
}

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
uint64_t *HandleSameColumnException(int *PParts, RelationMD *Binding1, IMData *imData) {
    //[relid][num of r]
    imData->visited[PParts[0]] = true;
    uint64_t * temp = imData->IMResColumnsForFilters[PParts[0]];
    uint64_t * tempres, *tempres1;
    uint64_t resNum=0;
    // 0 is rel1          1 is 1st col           2 is rel2(1)    3 is 2nd col
    if (imData->IMResColumnsForFilters[PParts[0]] != nullptr){
        tempres = new uint64_t[temp[1]];
        for (uint64_t i = 2; i < temp[1]+2; ++i) {
            if(Binding1->RelationSerialData[Binding1->RowsNum * PParts[1] + temp[i]] == Binding1->RelationSerialData[Binding1->RowsNum * PParts[3] + temp[i]]){
                tempres[resNum]=temp[i];
                resNum++;
            }
        }
        delete [] imData->IMResColumnsForFilters[PParts[0]];
    } else{
        tempres = new uint64_t[Binding1->RowsNum];
        for (uint64_t i = 0; i < Binding1->RowsNum; ++i) {
            if(Binding1->RelationSerialData[Binding1->RowsNum * PParts[1] + i] == Binding1->RelationSerialData[Binding1->RowsNum * PParts[3] + i]){
                tempres[resNum]=i;
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
    //return rvalue;
    if(PParts[0] == PParts[2])return nullptr;
    imData->visited[PParts[2]] = true;
    temp = imData->IMResColumnsForFilters[PParts[2]];
    resNum=0;
    if (imData->IMResColumnsForFilters[PParts[2]] != nullptr){
        tempres = new uint64_t[temp[1]];
        for (uint64_t i = 2; i < temp[1]+2; ++i) {
            if(Binding1->RelationSerialData[temp[i] + Binding1->RowsNum * PParts[1]] == Binding1->RelationSerialData[temp[i] + Binding1->RowsNum * PParts[3]]){
                tempres[resNum]=temp[i];
                resNum++;
            }
        }
        delete [] imData->IMResColumnsForFilters[PParts[2]];
    } else{
        tempres = new uint64_t[Binding1->RowsNum];
        for (uint64_t i = 0; i < Binding1->RowsNum; ++i) {
            if(Binding1->RelationSerialData[Binding1->RowsNum * PParts[1] + i] == Binding1->RelationSerialData[Binding1->RowsNum * PParts[3] + i]){
                tempres[resNum]=i;
                resNum++;
            }
        }
    }
    rvalue = new uint64_t[resNum+2];
    rvalue[0] = PParts[2];
    rvalue[1] = resNum;
    for (int j = 2; j < resNum+2; ++j) {
        rvalue[j] = tempres[j-2];
    }
    imData->IMResColumnsForFilters[PParts[2]] = rvalue;

    delete [] tempres;


}
void putInImResults(uint64_t *buffer, uint64_t *Results, uint64_t index, int iterationIndex) {
    for (uint64_t i = 0; i < iterationIndex; ++i) {
        Results[index+i] = buffer[i];
    }
}
void printTuple(uint64_t * buffer, int bufferSize){
    for (int i = 0; i < bufferSize; ++i) {
        cout<< buffer[i]<<" ";
    }
    cout<<endl;
}
void putInBuffer(uint64_t *buffer, uint64_t index, uint64_t *ImRes, int iterationIndex) {
    for (uint64_t i = 0; i < iterationIndex ; ++i) {
        buffer[i] = ImRes[index + i];
    }
}

void copyToNewIMResults(uint64_t *newResults, uint64_t iterationIndex, const uint64_t *buffer, int index) {
    for (uint64_t i = 0; i < iterationIndex; ++i) {
        newResults[index+i] = buffer[i];
    }
}
void AddToData(IMData *data, List *start, uint64_t numOfTuples, uint64_t bindingR, uint64_t bindingS) {

    uint64_t hashTableSize = (numOfTuples/3)+1;
    if(data->IMResColumnsForJoin == nullptr){
        data->visitedJoint[bindingR]=true;
        data->visitedJoint[bindingS]=true;
        data->Map[0] = bindingR;
        data->Map[1] = bindingS;
        data->HashTable[0] = new listNode*[hashTableSize];
        data->HashTable[1] = new listNode*[hashTableSize];
        initHash(data->HashTable[0], hashTableSize);
        initHash(data->HashTable[1], hashTableSize);
        data->IMResColumnsForJoin = new uint64_t[numOfTuples*2];
        uint64_t index = 0;
        List *temp = start;
        while(temp != NULL) {
            for (uint64_t i = 0; i < temp->index; i++) {
                data->IMResColumnsForJoin[index] = temp->rowIDR[i];
                data->IMResColumnsForJoin[index + 1] = temp->rowIDS[i];
                insertInHash(data->HashTable[0], hashTableSize, temp->rowIDR[i], &data->IMResColumnsForJoin[index]);
                insertInHash(data->HashTable[1], hashTableSize, temp->rowIDS[i], &data->IMResColumnsForJoin[index]);
                index += 2;
            }
            temp = temp->next;
        }
        data->numOfPleiades = numOfTuples;
        return;
    }
    int numOfColsInTuple = getPleiada(data->visitedJoint, data->numOfBindings);
    if(data->visitedJoint[bindingR] ^ data->visitedJoint[bindingS]){
        int commonColumn, commonOfPair;
        if(!data->visitedJoint[bindingR]){
            data->visitedJoint[bindingR] = true;
            data->Map[numOfColsInTuple] = bindingR;
            //commonColumn stoixeio einai ayto poy to rowd[0] tou einai sto map. to map exei se poia 8esh pleiadaw emfanizetai. rara
            commonColumn = getFromMap(data->Map, data->numOfBindings, bindingS);
            commonOfPair = 1;

        }
        if(!data->visitedJoint[bindingS]){
            data->visitedJoint[bindingS] = true;
            data->Map[numOfColsInTuple] = bindingS;
            commonColumn = getFromMap(data->Map, data->numOfBindings, bindingR);
            commonOfPair = 0;
        }
        uint64_t pleiades_new=0;
        int newNumOfColsInTuple = numOfColsInTuple + 1;
        uint64_t *temp = new uint64_t[newNumOfColsInTuple];
        uint64_t * Results;
        uint64_t newSize = 0;
        List *tempList = start;
        while (tempList != NULL) {
            for(uint64_t i = 0 ; i < tempList->index; i++) {
                //for (uint64_t j = 0; j < data->numOfPleiades; j++) {
                    uint64_t pos;
                    if (commonOfPair == 0) {
                        pos = findBucket(tempList->rowIDR[i], (data->numOfPleiades/3)+1);
                        listNode *tempListNode = data->HashTable[commonColumn][pos];
                        while(tempListNode != NULL) {
                            if(tempListNode->number == tempList->rowIDR[i])
                                newSize++;
                            tempListNode = tempListNode->next;
                        }
                    } else {
                        pos = findBucket(tempList->rowIDS[i], (data->numOfPleiades/3)+1);
                        listNode *tempListNode = data->HashTable[commonColumn][pos];
                        while(tempListNode != NULL) {
                            if(tempListNode->number == tempList->rowIDS[i])
                                newSize++;
                            tempListNode = tempListNode->next;
                        }
                    }

            }
            tempList = tempList->next;
        }
        hashTableSize = (newSize/3)+1;
        listnode ***tempHashTable = new listnode **[data->numOfBindings];

        for(int i = 0; i < numOfColsInTuple; i++)
        {
            if(i!=commonColumn){
                deleteHashTable(data->HashTable[i], (data->numOfPleiades/3)+1);
                delete[] data->HashTable[i];
            }
        }
        for(int i = 0; i < newNumOfColsInTuple; i++)
        {
            tempHashTable[i] = new listNode *[hashTableSize];
            initHash(tempHashTable[i], hashTableSize);
        }

        Results = new uint64_t[newSize*newNumOfColsInTuple];
        tempList = start;
        while (tempList != NULL) {
            for (uint64_t i = 0; i < tempList->index; i++) {
//                for (uint64_t j = 0; j < data->numOfPleiades; j++) {
                    uint64_t pos;
                    if (commonOfPair == 0) {
                        pos = findBucket(tempList->rowIDR[i], (data->numOfPleiades/3)+1);
                        listNode *tempListNode = data->HashTable[commonColumn][pos];
                        while(tempListNode != NULL) {
                            if(tempListNode->number == tempList->rowIDR[i]) {
                                copyToBuffer(temp, tempListNode->pointer, numOfColsInTuple,
                                             tempList->rowIDS[i]);
                                //ftemp = data->IMResColumnsForJoin[j*numOfColsInTuple]
                                copyToNewIMResults(Results, newNumOfColsInTuple, temp,
                                                   (pleiades_new * newNumOfColsInTuple));
                                for(int i = 0; i < newNumOfColsInTuple; i++)
                                {
                                    insertInHash(tempHashTable[i], hashTableSize, temp[i], &Results[(pleiades_new * newNumOfColsInTuple)]);
                                }
                                pleiades_new++;
                            }
                            tempListNode = tempListNode->next;
                        }
//                        if (tempList->rowIDR[i] == data->IMResColumnsForJoin[j * numOfColsInTuple + commonColumn]) {
//                            //copy oloklhrh numOfColsInTuple in ptemp, put fresh[!commonOfPair][i]sto teleytaio stoixeio ths pleiadas_new
//                            copyToBuffer(temp, data->IMResColumnsForJoin, j, numOfColsInTuple, newNumOfColsInTuple,
//                                         tempList->rowIDS[i]);
//                            //ftemp = data->IMResColumnsForJoin[j*numOfColsInTuple]
//                            copyToNewIMResults(Results, newNumOfColsInTuple, temp,
//                                               (pleiades_new * newNumOfColsInTuple));
//                            pleiades_new++;
//                            //put in new IMresults
//                        }
                    } else{
                        pos = findBucket(tempList->rowIDS[i], (data->numOfPleiades/3)+1);
                        listNode *tempListNode = data->HashTable[commonColumn][pos];
                        while(tempListNode != NULL) {
                            if(tempListNode->number == tempList->rowIDS[i]) {
                                copyToBuffer(temp, tempListNode->pointer, numOfColsInTuple,
                                             tempList->rowIDR[i]);
                                //ftemp = data->IMResColumnsForJoin[j*numOfColsInTuple]
                                copyToNewIMResults(Results, newNumOfColsInTuple, temp,
                                                   (pleiades_new * newNumOfColsInTuple));
                                for(int i = 0; i < newNumOfColsInTuple; i++)
                                {
                                    insertInHash(tempHashTable[i], hashTableSize, temp[i], &Results[(pleiades_new * newNumOfColsInTuple)]);
                                }
                                pleiades_new++;
                            }
                            tempListNode = tempListNode->next;
                        }
                    }
//                }
            }
            tempList = tempList->next;
        }
        deleteHashTable(data->HashTable[commonColumn], (data->numOfPleiades/3)+1);
        delete[] data->HashTable[commonColumn];
        delete[] data->HashTable;
        data->HashTable = tempHashTable;
        delete [] temp;
        data->numOfPleiades = pleiades_new;
        delete [] data->IMResColumnsForJoin;
        data->IMResColumnsForJoin = Results;
        return;
    }
    //last case is if they both are in imres

    if(!data->visitedJoint[bindingR] && !data->visitedJoint[bindingS]) {
        data->visitedJoint[bindingR] = true;
        data->Map[numOfColsInTuple] = bindingR;

        data->visitedJoint[bindingS] = true;
        data->Map[numOfColsInTuple + 1] = bindingS;
        int newNumOfColsInTuple = numOfColsInTuple + 2;
        uint64_t *temp = new uint64_t[newNumOfColsInTuple];
        uint64_t *Results = new uint64_t[data->numOfPleiades * (numOfTuples) * newNumOfColsInTuple];
        uint64_t pleiades_new = 0;
        List *tempList = start;
        listnode ***tempHashTable = new listnode **[data->numOfBindings];
        hashTableSize = ((data->numOfPleiades * (numOfTuples))/3)+1;
        for(int i = 0; i < numOfColsInTuple; i++)
        {
            deleteHashTable(data->HashTable[i], (data->numOfPleiades/3)+1);
            delete[] data->HashTable[i];
        }
        delete[] data->HashTable;
        for(int i = 0; i < newNumOfColsInTuple; i++)
        {
            tempHashTable[i] = new listNode *[hashTableSize];
            initHash(tempHashTable[i], hashTableSize);
        }
        while (tempList != nullptr) {
            for (uint64_t i = 0; i < tempList->index; i++) {
                for (uint64_t j = 0; j < data->numOfPleiades; j++) {
                    putInBuffer(temp, j * numOfColsInTuple, data->IMResColumnsForJoin,
                                numOfColsInTuple);
                    temp[numOfColsInTuple] = tempList->rowIDR[i];
                    temp[numOfColsInTuple+1] = tempList->rowIDS[i];
                    putInImResults(temp, Results, pleiades_new * newNumOfColsInTuple,
                                   newNumOfColsInTuple);
                    for(int i = 0; i < newNumOfColsInTuple; i++)
                    {
                        insertInHash(tempHashTable[i], hashTableSize, temp[i], &Results[(pleiades_new * newNumOfColsInTuple)]);
                    }
                    pleiades_new++;
                }
            }
            tempList = tempList->next;
        }
        data->HashTable = tempHashTable;
        delete[] temp;
        data->numOfPleiades = pleiades_new;
        delete[] data->IMResColumnsForJoin;
        data->IMResColumnsForJoin = Results;
        return;
    }


    //last case is if they are both not in imres.
}

void BothExistInImJoinException(IMData *data, RelationMD *BindingR, RelationMD *BindingS, int *PParts)
{
    int numOfColsInTuple = getPleiada(data->visitedJoint, data->numOfBindings);
    int col1 = getFromMap(data->Map, data->numOfBindings, PParts[0]), col2 = getFromMap(data->Map, data->numOfBindings, PParts[2]);
    uint64_t pleiades_new =0;
    for (uint64_t j = 0; j < data->numOfPleiades; j++) {
        uint64_t indexInBinding1, indexInBinding2;
        indexInBinding1 = data->IMResColumnsForJoin[j * numOfColsInTuple + col1];
        indexInBinding2 = data->IMResColumnsForJoin[j * numOfColsInTuple + col2];

        if ( BindingR->RelationSerialData[BindingR->RowsNum*PParts[1] + indexInBinding1] ==
             BindingS->RelationSerialData[BindingS->RowsNum*PParts[3] + indexInBinding2]) {
            pleiades_new++;
        }
    }
    uint64_t *temp = new uint64_t[numOfColsInTuple];
    uint64_t *Results = new uint64_t[pleiades_new * numOfColsInTuple];
    listnode ***tempHashTable = new listnode **[data->numOfBindings];
    uint64_t hashTableSize = ((pleiades_new)/3)+1;
    for(int i = 0; i < numOfColsInTuple; i++)
    {
        deleteHashTable(data->HashTable[i], (data->numOfPleiades/3)+1);
        delete[] data->HashTable[i];
        tempHashTable[i] = new listNode *[hashTableSize];
        initHash(tempHashTable[i], hashTableSize);
    }
    delete[] data->HashTable;
    pleiades_new = 0;
    for (uint64_t j = 0; j < data->numOfPleiades; j++) {
        uint64_t indexInBinding1, indexInBinding2;
        indexInBinding1 = data->IMResColumnsForJoin[j * numOfColsInTuple + col1];
        indexInBinding2 = data->IMResColumnsForJoin[j * numOfColsInTuple + col2];
        if ( BindingR->RelationSerialData[BindingR->RowsNum*PParts[1] + indexInBinding1] ==
             BindingS->RelationSerialData[BindingS->RowsNum*PParts[3] + indexInBinding2]) {

            putInBuffer(temp, j * numOfColsInTuple, data->IMResColumnsForJoin, numOfColsInTuple);
//                    printTuple(temp, getPleiada(data->visitedJoint, data->numOfBindings));
            putInImResults(temp, Results, pleiades_new * numOfColsInTuple,
                           getPleiada(data->visitedJoint, data->numOfBindings));
            for(int i = 0; i < numOfColsInTuple; i++)
            {
                insertInHash(tempHashTable[i], hashTableSize, temp[i], &Results[(pleiades_new * numOfColsInTuple)]);
            }
            pleiades_new++;
        }
    }
    data->HashTable = tempHashTable;
    delete[] temp;
    data->numOfPleiades = pleiades_new;
    delete[] data->IMResColumnsForJoin;
    data->IMResColumnsForJoin = Results;
    return;
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
    auto * list = new List1;
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

    }
    workload.close();
}



uint64_t minInt(const uint64_t a, const uint64_t i) {
    return ((a<i)? a: i);
}

uint64_t maxInt(const uint64_t a, const uint64_t i) {
    return ((a>i)? a: i);
}

void updateStats(QueryStats *statistics, const string &Predicate) {
    uint64_t lowerA, upperA;
    uint64_t fA, distinctA;
    short SwitchValue = typeOfPredicate(Predicate);
    int  * PParts;
    uint64_t * PPartsF, temp7;
    double temp1, temp2, temp3, temp4, temp5, temp6;
    switch (SwitchValue){
        case FILTER:
            PPartsF = getNumericalValuePredicateParts(Predicate);
            switch (PPartsF[2]){

                case '=': {
                    statistics->stats[PPartsF[0]][PPartsF[1]].upperA = statistics->stats[PPartsF[0]]->lowerA = PPartsF[3]; // la = ua = k
                    fA = statistics->stats[PPartsF[0]][PPartsF[1]].fA;
                    statistics->stats[PPartsF[0]][PPartsF[1]].fA = (statistics->stats[PPartsF[0]]->fA /
                                                                    statistics->stats[PPartsF[0]]->distinctA);
                    uint64_t distinctAA = statistics->stats[PPartsF[0]][PPartsF[1]].distinctA;
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
                    distinctA = statistics->stats[PPartsF[0]][PPartsF[1]].distinctA;
                    statistics->stats[PPartsF[0]][PPartsF[1]].lowerA = maxInt(statistics->stats[PPartsF[0]][PPartsF[1]].lowerA, PPartsF[3]);

                    temp1 = (double)(statistics->stats[PPartsF[0]][PPartsF[1]].upperA - statistics->stats[PPartsF[0]][PPartsF[1]].lowerA)*(double)distinctA;
                    temp2 =(double) (statistics->stats[PPartsF[0]][PPartsF[1]].upperA-lowerA);
                    temp3 = temp1/temp2;
                    statistics->stats[PPartsF[0]][PPartsF[1]].distinctA = (uint64_t) temp3;

                    temp1 = (double)(statistics->stats[PPartsF[0]][PPartsF[1]].upperA - statistics->stats[PPartsF[0]][PPartsF[1]].lowerA);
                    temp2 = (double)(statistics->stats[PPartsF[0]][PPartsF[1]].upperA - lowerA);
                    temp3 = temp1/temp2;
                    temp4 = (double)statistics->stats[PPartsF[0]][PPartsF[1]].fA*temp3;
                    statistics->stats[PPartsF[0]][PPartsF[1]].fA = (uint64_t)temp4;


                    for (int i = 0; i < statistics->TuplesPerBinding[PPartsF[0]]; ++i) { // foreach column in relation
                        if (i == PPartsF[1]){
                            continue;
                        }
                        temp1 = (double)(1 -(double)statistics->stats[PPartsF[0]][PPartsF[1]].fA/(double)fA);
                        temp2 = (double)statistics->stats[PPartsF[0]][i].fA/(double)statistics->stats[PPartsF[0]][i].distinctA;
                        temp3 = pow(temp1,temp2);
                        temp4 = (1 - temp3);
                        temp5 = statistics->stats[PPartsF[0]][i].distinctA*temp4;
                        temp7 = (uint64_t)temp5;
                        statistics->stats[PPartsF[0]][i].distinctA = temp7;
                        //statistics->stats[PPartsF[0]][i].distinctA = statistics->stats[PPartsF[0]][i].distinctA*(1 - ( pow(, )));
                        statistics->stats[PPartsF[0]][i].fA = statistics->stats[PPartsF[0]][PPartsF[1]].fA;
                    }
                    break;
                case '<':
                    upperA = statistics->stats[PPartsF[0]][PPartsF[1]].upperA;
                    fA = statistics->stats[PPartsF[0]][PPartsF[1]].fA;
                    distinctA = statistics->stats[PPartsF[0]][PPartsF[1]].distinctA;
                    statistics->stats[PPartsF[0]][PPartsF[1]].upperA = minInt(statistics->stats[PPartsF[0]][PPartsF[1]].upperA, PPartsF[3]);
                    temp1 = (double)(statistics->stats[PPartsF[0]][PPartsF[1]].upperA - statistics->stats[PPartsF[0]][PPartsF[1]].lowerA)*(double)distinctA;
                    temp2 =(double) (upperA-statistics->stats[PPartsF[0]][PPartsF[1]].lowerA);
                    temp3 = temp1/temp2;
                    statistics->stats[PPartsF[0]][PPartsF[1]].distinctA = (uint64_t)temp3;

                    temp2 = (double)(statistics->stats[PPartsF[0]][PPartsF[1]].upperA - statistics->stats[PPartsF[0]][PPartsF[1]].lowerA);
                    temp3 = temp1/temp2;
                    temp4 = (double)statistics->stats[PPartsF[0]][PPartsF[1]].fA*temp3;
                    statistics->stats[PPartsF[0]][PPartsF[1]].fA = (uint64_t)temp4;

                    for (int i = 0; i < statistics->TuplesPerBinding[PPartsF[0]]; ++i) { // foreach column in relation
                        if (i == PPartsF[1]){
                            continue;
                        }
                        temp1 = (double)(1 -(double)statistics->stats[PPartsF[0]][PPartsF[1]].fA/(double)fA);
                        temp2 = (double)statistics->stats[PPartsF[0]][i].fA/(double)statistics->stats[PPartsF[0]][i].distinctA;
                        temp3 = pow(temp1,temp2);
                        temp4 = (1 - temp3);
                        temp5 = statistics->stats[PPartsF[0]][i].distinctA*temp4;
                        temp7 = (uint64_t)temp5;
                        statistics->stats[PPartsF[0]][i].distinctA = temp7;
                        statistics->stats[PPartsF[0]][i].fA = statistics->stats[PPartsF[0]][i].fA;
                    }
                    break;
                default:
                    cout << "This is not a valid Predicate. Aborting...\n";
            }
            delete [] PPartsF;
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
                temp1 = (double)(1 -(double)statistics->stats[PParts[0]][PParts[1]].distinctA/(double)distinctA);
                temp2 = (double)((double)statistics->stats[PParts[0]][i].fA/(double)statistics->stats[PParts[0]][i].distinctA);
                temp3 = pow(temp1,temp2);
                temp4 = (1 - temp3);
                temp5 = statistics->stats[PParts[0]][i].distinctA*temp4;
                temp7 = (uint64_t)temp5;
                statistics->stats[PParts[0]][i].distinctA = temp7;

                statistics->stats[PParts[0]][i].fA = statistics->stats[PParts[0]][PParts[1]].fA;
            }
            distinctA = statistics->stats[PParts[2]][PParts[3]].distinctA;

            //new upper for both relcolumns in join is min of their old two upper

            //for rest of columns in both
            for (int i = 0; i < statistics->TuplesPerBinding[PParts[2]]; ++i) { // foreach column in relation
                if (i == PParts[3]){
                    continue;
                }
                temp1 = (double)(1 -(double)statistics->stats[PParts[2]][PParts[3]].distinctA/(double)distinctA);
                temp2 = (double)((double)statistics->stats[PParts[2]][i].fA/(double)statistics->stats[PParts[2]][i].distinctA);
                temp3 = pow(temp1,temp2);
                temp4 = (1 - temp3);
                temp5 = statistics->stats[PParts[2]][i].distinctA*temp4;
                temp7 = (uint64_t)temp5;

            }
            delete [] PParts;
            break;
        default:
            cout << "This is not a valid Predicate. Aborting...\n";
    }
}

void deleteStats(RelationCS **stats, int numOfBindings, QueryStats Qstats) {
    delete [] Qstats.TuplesPerBinding;
    for (int i = 0; i < numOfBindings; ++i) {
        delete [] stats[i];
    }
    delete [] stats;
}

RelationCS **initStats(RelationMD **bindings, int numOfBindings, QueryStats *QStats) {
    auto ** rvalue = new RelationCS*[numOfBindings];
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
}



// The main function that prints all combinations of
// size r in arr[] of size n. This function mainly
// uses combinationUtil()

/* arr[]  ---> Input Array
   n      ---> Size of input array
   r      ---> Size of a combination to be printed
   index  ---> Current index in data[]
   data[] ---> Temporary array to store current combination
   i      ---> index of current element in arr[]     */

// Driver program to test above functions
