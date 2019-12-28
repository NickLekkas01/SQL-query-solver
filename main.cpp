#include <iostream>
using namespace std;
#include <fstream>
//#include "DBRelationsReader.h"
#include "QueryEditor.h"
#define NUMBATCHES 5

#include <ctime>
//void addFilterToIMResults(IMData * data, uint64_t *results, int binding){
//    data->IMResColumnsForFilters[binding] = results;
//}

int main(int argc, char *argv[]) {
    clock_t start = clock();
    List1* debugBatches[NUMBATCHES],  * curr1;
    ListNode * curr;
    Data * data = getData(argv[1]);

    //getLists(debugBatches,NUMBATCHES,argv[2]);
    JobExecutor(argv[2] , data);
    cout << "SUP\n";
//    for (auto & debugBatche : debugBatches) {
//        batchExecutor(debugBatche, data);
//    }

    DeleteData(data);
    clock_t stop = clock();
    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
    printf("\nTime elapsed: %.5f\n", elapsed);
    return 0;
}