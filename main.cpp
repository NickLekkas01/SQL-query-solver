#include <iostream>
#include <cstdlib>
using namespace std;
#include <fstream>

typedef struct RelationMetadata{
    uint64_t TuplesNum,
             RowsNum,
             *RelationSerialData;
}RelationMD;
void printDefinedStruct(RelationMD * relationMd){
    cout << "testing final product\n";
    cout << relationMd->RowsNum << " "<< relationMd->TuplesNum<<endl;
    for(uint64_t i =0 ; i < relationMd->RowsNum *relationMd->TuplesNum;i++){
        if(i%relationMd->RowsNum == 0){
            //getchar();
        }
        cout << relationMd->RelationSerialData[i]<<endl;

    }

}
int getInitFileLines(const string& fileName){
    ifstream file;
    int lineCount = 0;
    //get file lines
    file.open(fileName);
    if(!file){
        cout<< "Couldn't open .init file\n";
        return 1;
    }
    string fileLine;
    //keep pulling lines until you either run out of lines or you get the keyword Done
    while (file >> fileLine && fileLine != "Done"){
        lineCount++;
        //cout << fileLine<< endl;
    }
    file.close();
    return lineCount;
    // end of file line pulling
}
int getRelationData(const string& fileName, RelationMD * currRelMD){
    ifstream file;

    file.open(fileName,ios::in|ios::binary);
    if(!file){
        cout<<"Couldn't open binary relation file\n";
    }
    uint64_t temp1;
    //cout << temp.get();
    int flag = 0;
    file.read((char*)&currRelMD->RowsNum, sizeof(temp1));
    cout << currRelMD->RowsNum<< endl;
    file.read((char*)&currRelMD->TuplesNum, sizeof(temp1));
    cout << currRelMD->TuplesNum<< endl;
    currRelMD->RelationSerialData = new uint64_t[(currRelMD->TuplesNum)*(currRelMD->RowsNum)];
    //getchar();
    uint64_t index = 0;
    while(file.good()){
        file.read((char*)&temp1, sizeof(temp1));
        currRelMD->RelationSerialData[index] = temp1;
        //cout << temp1<< endl;
        //cout << currRelMD->RelationSerialData[index]<<endl;
        index++;

    }
    file.close();
}
int getInitFileNames(const string& fileName){
    ifstream file;
    string fileLine;

    cout << "About to open .init file\n";
    int lineCount = getInitFileLines(fileName);
    // make a rel metadata array with line count spaces
    RelationMD * AllRelMD = new RelationMD[lineCount];
    //start of getting the names
    file.open(fileName);
    if(!file){
        cout<< "Couldn't open .init file\n";
        return 1;
    }
    int index = 0;

    while (file >> fileLine && fileLine != "Done"){
        cout << fileLine<< endl;
        // send filename to get relation Data
        getRelationData(fileLine,&AllRelMD[index]);
        printDefinedStruct(&AllRelMD[index]);
        index ++;
    }
    //end of getting the names
    return 0;
}

int main(int argc, char *argv[]) {
    std::cout << "Hello, World!" << std::endl;
    RelationMD rel;
    //read from the first file all the filenames (string)
    //read from
    getInitFileNames(argv[1]);
    string path = "/home/athena/CLionProjects/tempproj/small/r0";
    //getRelationData(path,&rel);
    //temp.read((char*)&temp1, sizeof(temp1));
    //temp.read((char*)&temp1, sizeof(temp1));
    //temp >> temp1;
    return 0;
}