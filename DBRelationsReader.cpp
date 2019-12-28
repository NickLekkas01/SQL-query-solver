#include "DBRelationsReader.h"
using namespace std;

void printDefinedStruct(RelationMD * relationMd){
    cout << "testing final product\n";
    cout << relationMd->RowsNum << " "<< relationMd->TuplesNum<<endl;
    cout << relationMd->name<<endl;
    //for(uint64_t i =0 ; i < relationMd->RowsNum *relationMd->TuplesNum;i++){

        //cout << relationMd->RelationSerialData[i]<<endl;

    //}

}
void DeleteData(Data * data){
    for (int i = 0; i < data->relationNum; ++i) {
        delete [] data->relationsData[i].RelationSerialData;
    }
    delete [] data->relationsData;
    delete data;
}
int getInitFileLines(const string& fileName){
    ifstream file, file1;
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
        file1.open(fileLine,ios::in|ios::binary);
        if(!file1){
            cout << "File could not be opened, will not be counted in number of relations\n";
        } else{
            lineCount++;

        }
        file1.close();
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
        // set relation uint values to 0, pointer to null
        currRelMD->RowsNum = 0;
        currRelMD->TuplesNum = 0;
        currRelMD->RelationSerialData = nullptr;
    }
    uint64_t temp1;
    file.read((char*)&currRelMD->RowsNum, sizeof(temp1));
    ///cout << currRelMD->RowsNum<< endl;
    file.read((char*)&currRelMD->TuplesNum, sizeof(temp1));
    //cout << currRelMD->TuplesNum<< endl;

    uint64_t space = (currRelMD->TuplesNum)*(currRelMD->RowsNum);
    currRelMD->RelationSerialData = new uint64_t[space];
    uint64_t index = 0;
    for (uint64_t i = 0 ; i < space ; i++ ){
        file.read((char*)&temp1, sizeof(temp1));
        currRelMD->RelationSerialData[index] = temp1;
        //cout << temp1<< endl;
        //cout << currRelMD->RelationSerialData[index]<<endl;
        index++;

    }
    currRelMD->name = fileName.substr( fileName.length() - 2 );
    file.close();
}
Data * getData(const string& fileName){
    ifstream file;
    string fileLine;
    Data * temp = new Data;
    //cout << "About to open .init file\n";
    int lineCount = getInitFileLines(fileName);
    // make a rel metadata array with line count spaces
    RelationMD * AllRelMD = new RelationMD[lineCount];
    //start of getting the names
    file.open(fileName);
    if(!file){
        //cout<< "Couldn't open .init file\n";
        return nullptr;
    }
    int index = 0;

    while (file >> fileLine && fileLine != "Done"){
        //cout << fileLine<< endl;
        // send filename to get relation Data
        getRelationData(fileLine,&AllRelMD[index]);
        //printDefinedStruct(&AllRelMD[index]);
        index ++;
    }
    //end of getting the names
    temp->relationNum = lineCount;
    temp->relationsData = AllRelMD;
    return temp;
    //return AllRelMD;
}
