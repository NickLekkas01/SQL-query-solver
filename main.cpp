#include <iostream>
using namespace std;
#include <fstream>
//#include "DBRelationsReader.h"
#include "QueryEditor.h"
#include <ctime>

//void addFilterToIMResults(IMData * data, uint64_t *results, int binding){
//    data->IMResColumnsForFilters[binding] = results;
//}

int main(int argc, char *argv[]) {
    std::cout << "Hello, World!" << std::endl;
    clock_t start = clock();

    //string temp = "3 0 1|0.2=1.0&0.1=2.0&0.2>3499|1.2 0.1", parts[3];
    //string x="0.2=1.0&0.1=2.0&0.2>3499";
    //getPredicateParts("3.2=1.1");
    //getNumericalValuePredicateParts("8.8>8");


    Data * data = getData(argv[1]);
    JobExecutor(argv[2], data);
    DeleteData(data);

    //Binded 0 , 1, 2, 3, 4
    // rowId's returned by filter or same relation column( Executed upfront)
    //say this is the column in question(RowIds passed by filter):
    //
    clock_t stop = clock();
    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
    printf("\nTime elapsed: %.5f\n", elapsed);
    return 0;
}