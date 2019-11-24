#include <iostream>
using namespace std;
#include <fstream>
#include "DBRelationsReader.h"
#include "QueryList.h"
#define QUERYPARTS 3
#include <cstring>

int getParts( string query, string * QueryParts){
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
RelationMD ** getBindings(string relations, Data * data){
    int numOfBindings = getNumOfRelations(relations);
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
void batchExecutor(List * batch, Data * data){
    ListNode * curr = batch->start;
    string QueryParts[QUERYPARTS];
    RelationMD ** Bindings;
    while (curr!= nullptr){
        getParts(curr->query, QueryParts);
        Bindings = getBindings(QueryParts[0], data);
        curr = curr->next;
    }
}
void JobExecutor(const string& queriesFile, Data * data ){
    ifstream workload(queriesFile);
    if(!workload){
        cout << "Couldn't open workload file";

    }
    List list;
    string query;
    while (getline(workload, query)){
        cout << query<< endl;
        //if query is F, send batch to process, reset list
        if(query == "F"){
            //send batch to process
            batchExecutor(&list, data);
            PrintList(&list);
            DeleteList(&list);
            list.start= nullptr;
            list.end = nullptr;
        }
        //add query to list
        AddToList(&list, query);

    }
    workload.close();

}
int main(int argc, char *argv[]) {
    std::cout << "Hello, World!" << std::endl;
    //JobExecutor("/home/athena/CLionProjects/tempproj/small/small.work", nullptr);
    string temp = "3 0 1|0.2=1.0&0.1=2.0&0.2>3499|1.2 0.1", parts[3];
    RelationMD ** rel;
    Data * data = getData(argv[1]);
    getParts(temp, parts);
    rel = getBindings(parts[0], data);
    for (int i = 0; i < 3 ; ++i) {
        printDefinedStruct(rel[i]);
    }
    DeleteData(data);
    return 0;
}