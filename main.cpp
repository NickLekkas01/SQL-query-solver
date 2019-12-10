#include <iostream>
using namespace std;
#include <fstream>
//#include "DBRelationsReader.h"
#include "QueryEditor.h"

int main(int argc, char *argv[]) {
    std::cout << "Hello, World!" << std::endl;
    //JobExecutor("/home/athena/CLionProjects/tempproj/small/small.work", nullptr);
    string temp = "3 0 1|0.2=1.0&0.1=2.0&0.2>3499|1.2 0.1", parts[3];
    string x="0.2=1.0&0.1=2.0&0.2>3499";
    ExecuteNumericalValueQuery("0.1=1000", nullptr,0);

    //RelationMD ** rel;
    //Data * data = getData(argv[1]);
    //JobExecutor("/home/athena/CLionProjects/tempproj/small/small.work", data);
    //DeleteData(data);
    return 0;
}