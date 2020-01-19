#include <iostream>
using namespace std;
#include "QueryEditor.h"
#include <ctime>



int main(int argc, char *argv[]) {
    std::cout << "Hello, World!" << std::endl;
    clock_t start = clock();

    Data * data = getData(argv[1]);
    JobExecutor(argv[2], data);
    DeleteData(data);

    clock_t stop = clock();
    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
    printf("\nTime elapsed: %.5f\n", elapsed);
    return 0;
}