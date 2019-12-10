//
// Created by athena on 8/12/19.
//

#include "QueryEditor.h"
using namespace std;
void printStrings(string * array , int size){
    for (int i = 0; i <size ; ++i) {
        cout << array[i]<<endl;
    }
}
void QueryExecutor(RelationMD **Bindings, string *Predicates, string *Projections, int numOfBindings, int numOfPredicates,
              int numOfProjections);


int getParts(string query, string * QueryParts){
    string delimiter = "|",
            token;
    size_t pos = 0, index = 0;
    while ((pos = query.find(delimiter)) != std::string::npos) {
        token = query.substr(0, pos);
        std::cout << token << std::endl;
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
        std::cout << token << std::endl;
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
    printStrings(temp, numOfPredicates);

    return temp;
    //return nullptr;
}
void batchExecutor(List * batch, Data * data){
    ListNode * curr = batch->start;
    string QueryParts[QUERYPARTS];
    RelationMD ** Bindings;
    string * Predicates;
    string * Projections = nullptr;
    int numOfBindings, numOfPredicates, numOfProjections=0;
    while (curr!= nullptr){
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
void QueryExecutor(RelationMD **Bindings, string *Predicates, string *Projections, int numOfBindings, int numOfPredicates, int numOfProjections) {
    for (int i = 0; i < numOfPredicates; ++i) {
        switch(typeOfPredicate(Predicates[i])){
            case 1: //we have a column value compare to number
                //get binded value(pointer to relation) , column value and operator, execute query
                ExecuteNumericalValueQuery(Predicates[i], Bindings, numOfBindings);
                break;
            case 2:
                //get binded values,aka the 2 pointers to relation, and the 2 column values, and operator, and execute query
                break;
        }
    }

}

void ExecuteNumericalValueQuery(string Predicate, RelationMD **Bindings, int numOfBindings) {
    size_t pos = 0, index = 0;
    //string delimiter = ".";
    string token;
    int PredicatePart[4], iter = Predicate.length();
    pos = Predicate.find(".");
    token = Predicate.substr(0, pos);
    PredicatePart[0] = stoi(token);
    Predicate.erase(0, pos + 1);
    cout << Predicate;
}

short typeOfPredicate(const string Predicate) {
    short iSpaces = 0;

    //Forgot to mention that strlen is incredibly ineffecient... store ahead of time.
    int strSize = Predicate.length();
    for(unsigned int iLoop = 0; iLoop < strSize; iLoop++ )
        if(Predicate[iLoop] == '.' )
            iSpaces++;
    return iSpaces;
}
void JobExecutor(const string &queriesFile, Data *data) {
    ifstream workload(queriesFile);
    if(!workload){
        cout << "Couldn't open workload file";

    }
    List list;
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
        PrintList(&list);

    }
    workload.close();

}
