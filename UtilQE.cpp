//
// Created by athena on 13/12/19.
//
#include "QueryEditor.h"
#include "UtilQE.h"
#include "QuickSort.h"
bool isTrue(uint64_t value1, int op, uint64_t value) {
    switch (op){
        case '=':
            if(value1 == value){
                return true;
            } else{ return false;}
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
    //return 0;
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
string *getPredicates(string predicates, int * predicatesNum) {
    int numOfPredicates = getNumOfPredicates(predicates);
    *predicatesNum = numOfPredicates;
    string * temp = new string[numOfPredicates];
    size_t pos = 0, index = 0;
    string delimiter = "&";
    string token;
    while ((pos = predicates.find(delimiter)) != std::string::npos) {
        token = predicates.substr(0, pos);
        //std::cout << token << std::endl;
        temp[index] = token; //todo change
        predicates.erase(0, pos + delimiter.length());
        //printDefinedStruct(&data->relationsData[stoi(token)]);

        index++;
    }
    temp[numOfPredicates - 1] = predicates;
    //printStrings(temp, numOfPredicates);

    string * temp2 = new string[numOfPredicates];
    index = 0;
    for(int i = 0; i < numOfPredicates; i++)
    {
        if(typeOfPredicate(temp[i]) == 1)
            temp2[index++] = temp[i];
    }
    for(int i = 0; i < numOfPredicates; i++)
    {
        if(typeOfPredicate(temp[i]) == 2 && temp[i][0] == temp[i][4])
            temp2[index++] = temp[i];
    }
    for(int i = 0; i < numOfPredicates; i++)
    {
        if(typeOfPredicate(temp[i]) == 2 && temp[i][0] != temp[i][4])
            temp2[index++] = temp[i];
    }
    delete[] temp;
    return temp2;
    //return nullptr;
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

            for (uint64_t i = PredicateParts[1]*RowsNum; i < (PredicateParts[1]+1)*RowsNum -1; ++i) {
                if(CorrespondingBinding[i] == PredicateParts[3]){
//                    cout << "query is satisfied!\n";
                    temp[resultsNum] = RowId;
                    resultsNum ++;
                }
                RowId++;
            }
            break;
        case '>':
            for (uint64_t i = PredicateParts[1]*RowsNum; i < (PredicateParts[1]+1)*RowsNum -1; ++i) {
                if(CorrespondingBinding[i] > PredicateParts[3]){
//                    cout << CorrespondingBinding[i]<<" query is satisfied!\n";
                    temp[resultsNum] = RowId;
                    resultsNum ++;
                }
                RowId++;
            }
            break;
        case '<':
            for (uint64_t i = PredicateParts[1]*RowsNum; i < (PredicateParts[1]+1)*RowsNum -1; ++i) {
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
    cout << resultsNum <<" resnum \n";

    delete [] temp;
    //return value array
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
    cout <<"Now processing Predicate "<< Predicate<<endl;

    size_t pos = 0, index = 0;
    string token;
    int PredicatePart[4], iter = Predicate.length();
    pos = Predicate.find('.');
    token = Predicate.substr(0, pos);
    PredicatePart[0] = stoi(token);
    //if the binding is bigger than the bindings num we have been given its invalid(?)
    Predicate.erase(0, pos + 1);

    if((pos = Predicate.find('=')) != string::npos){
        cout <<"Is isothta\n";
        token = Predicate.substr(0, pos);
        PredicatePart[1] = stoi(token);
        //if pp[1] is larger than bindings [pp[0]]-> numtuples is invalid
        PredicatePart[2] = '=';
    }else if((pos = Predicate.find('>')) != string::npos){
        cout <<"Is column > num\n";
        token = Predicate.substr(0, pos);
        PredicatePart[1] = stoi(token);
        PredicatePart[2] = '>';
    }else if((pos = Predicate.find('<')) != string::npos){
        cout <<"Is column < num\n";
        token = Predicate.substr(0, pos);
        PredicatePart[1] = stoi(token);
        PredicatePart[2] = '<';
    }else{
        cout <<"Unknown operator \n";
    }
    Predicate.erase(0, pos +1 );
    //cout << Predicate<<endl;
    PredicatePart[3] = stoi(Predicate);
    for (int i : PredicatePart) {
        cout << i<<endl;
    }
    //pp[0] is binding[pp[0]], pp[1] is which column we are checking, pp[2] is the op val, pp[3] is the comparing value
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

        if(isTrue(Binding->RelationSerialData[ExistingIMResults[i] * ColumnId],Op,value)){
            temp[resNums] = ExistingIMResults[i];
            resNums++;
        }
    }
    uint64_t * rvalue = new uint64_t[resNums+2];
    rvalue[0] = ExistingIMResults[0];
    rvalue[1] = resNums;
    for (size_t i = 0; i < resNums; i++) {
        //std::cout << temp[i] << endl;
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
    uint64_t * temp = new uint64_t[data->numOfPleiades];
    temp[0] = -1;
    for (uint64_t i = 0; i < data->numOfPleiades; ++i) {
        //#ifdef DEBUG
        //cout << data->IMResColumnsForJoin[i*pleiada_size + column];
        //#endif
        insertInResult(temp, data->IMResColumnsForJoin[i*pleiada_size + pos],&resNum);

    }
    relation->num_tuples = resNum;
    relation->tuples = new Tuple[resNum];
    for (uint64_t j = 0; j < resNum; ++j) {
        relation->tuples[j].payload = temp[j];
        relation->tuples[j].key = binding->RelationSerialData[column*binding->RowsNum + temp[j]];
    }
    delete []temp;
}

bool insertInResult(uint64_t *pInt, uint64_t element, uint64_t *iterationIndex) {
    int pos;
    uint64_t newPos =-1;
    if((pos = binarySearch(pInt, 0, *iterationIndex, element, &newPos)) == -1){
        //insert in new pos
        insertInNewPos(pInt, element, newPos, *iterationIndex);
        (*iterationIndex)++;
    }
    return false;
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
        uint64_t * Results;
//        if(data->numOfPleiades* (numOfTuples-1) * newNumOfColsInTuple <3759830160){
//
//            Results = new uint64_t[data->numOfPleiades* (numOfTuples-1) * newNumOfColsInTuple];
//        }else {
//            Results = new uint64_t[1000000];
//        }
        uint64_t newSize = 0;
        for(uint64_t i = 1 ; i < numOfTuples; i++) {
            for (uint64_t j = 0; j < data->numOfPleiades; j++) {
                if (fresh[commonOfPair][i] == data->IMResColumnsForJoin[j * numOfColsInTuple + commonColumn])
                    newSize++;
            }
        }
        Results = new uint64_t[newSize*newNumOfColsInTuple];
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

    if(data->visitedJoint[RowIDS1[0]] && data->visitedJoint[RowIDS2[0]]){
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
        return;

    }

    if(!data->visitedJoint[RowIDS1[0]] && !data->visitedJoint[RowIDS2[0]]){
        data->visitedJoint[RowIDS1[0]] = true;
        data->Map[numOfColsInTuple] = RowIDS1[0];

        data->visitedJoint[RowIDS2[0]] = true;
        data->Map[numOfColsInTuple+1] = RowIDS2[0];
        int newNumOfColsInTuple = numOfColsInTuple + 2;
        uint64_t *temp = new uint64_t[newNumOfColsInTuple];
        uint64_t *Results = new uint64_t[data->numOfPleiades* (numOfTuples-1) * newNumOfColsInTuple];
        uint64_t pleiades_new = 0;
        for(uint64_t i = 1 ; i < numOfTuples; i++){
            for(uint64_t j = 0; j < data->numOfPleiades; j++){
                putInBuffer(temp, pleiades_new, data->IMResColumnsForJoin, getPleiada(data->visited, data->numOfBindings));
                putInImResults(temp, Results, pleiades_new, getPleiada(data->visited, data->numOfBindings));
                pleiades_new++;
            }
        }
    }


    //last case is if they are both not in imres.
}
