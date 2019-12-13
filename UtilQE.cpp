//
// Created by athena on 13/12/19.
//
#include "QueryEditor.h"
#include "UtilQE.h"
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

    return temp;
    //return nullptr;
}

uint64_t * getResults(uint64_t *CorrespondingBinding, int RowsNum ,const int PredicateParts[4]) {
    uint64_t * temp = new uint64_t[RowsNum];
    uint64_t RowId = 0, resultsNum = 0;
    switch (PredicateParts[2]){
        case '=':

            for (uint64_t i = PredicateParts[1]*RowsNum; i < (PredicateParts[1]+1)*RowsNum -1; ++i) {
                if(CorrespondingBinding[i] == PredicateParts[3]){
                    cout << "query is satisfieδ!\n";
                    temp[resultsNum] = RowId;
                    resultsNum ++;
                }
                RowId++;
            }
            break;
        case '>':
            for (uint64_t i = PredicateParts[1]*RowsNum; i < (PredicateParts[1]+1)*RowsNum -1; ++i) {
                if(CorrespondingBinding[i] > PredicateParts[3]){
                    cout << CorrespondingBinding[i]<<" query is satisfieδ!\n";
                    temp[resultsNum] = RowId;
                    resultsNum ++;
                }
                RowId++;
            }
            break;
        case '<':
            for (uint64_t i = PredicateParts[1]*RowsNum; i < (PredicateParts[1]+1)*RowsNum -1; ++i) {
                if(CorrespondingBinding[i] < PredicateParts[3]){
                    cout << "query is satisfieδ!\n";
                    temp[resultsNum] = RowId;
                    resultsNum ++;
                }
                RowId++;
            }
            break;
    }
    //allocate return value , 1d array with
    uint64_t * rvalue = new uint64_t[resultsNum +2];
    rvalue[1] = resultsNum;
    for (size_t i = 0; i < resultsNum; i++) {
        //std::cout << temp[i] << endl;
        rvalue[i+1] = temp[i];
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
        //ιφ this meetws the query,

        if(isTrue(Binding->RelationSerialData[ExistingIMResults[i] * ColumnId],Op,value)){
            temp[i-2] = ExistingIMResults[i];
            resNums++;
        }
    }
    uint64_t * rvalue = new uint64_t[resNums+2];
    rvalue[1] = resNums;
    for (size_t i = 0; i < resNums; i++) {
        //std::cout << temp[i] << endl;
        rvalue[i+1] = temp[i];
        //copy values to returv value array
    }
    delete [] temp;
    return rvalue;
    //return temp;
}



//#include "UtilQE.h"
