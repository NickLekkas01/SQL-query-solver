#include "DBRelationsReader.h"
using namespace std;

void printDefinedStruct(RelationMD * relationMd){
    cout << "testing final product\n";
    cout << relationMd->RowsNum << " "<< relationMd->TuplesNum<<endl;
    cout << relationMd->name<<endl;

}
void DeleteData(Data * data){
    for (int i = 0; i < data->relationNum; ++i) {
        delete [] data->relationsData[i].RelationSerialData;
        delete [] data->relationsData[i].statistics;
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
void initTemp(bool *pInt, uint64_t pleiades) {
    for (uint64_t i = 0; i < pleiades; ++i) {
        pInt[i] = false;
    }

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
    currRelMD->statistics = new RelationColumnStats[currRelMD->TuplesNum];
    for (int k = 0; k < currRelMD->TuplesNum; ++k) {
        currRelMD->statistics[k].fA = (double)currRelMD->RowsNum;
    }
    //cout << currRelMD->TuplesNum<< endl;
    uint64_t alloc;
    uint64_t space = (currRelMD->TuplesNum)*(currRelMD->RowsNum);
    currRelMD->RelationSerialData = new uint64_t[space];
    uint64_t j = 0, max = 0, min = -1, distinct = 0; // negative value in unsigned guarantees max value
    for (uint64_t i = 0 ; i < space ; i++ ){
        file.read((char*)&temp1, sizeof(temp1));
        currRelMD->RelationSerialData[i] = temp1;
        if(temp1 <min)min = temp1;
        if(temp1>max)max = temp1;
        if((i+1)%currRelMD->RowsNum == 0 ){
            currRelMD->statistics[j].lowerA = min;
            currRelMD->statistics[j].upperA = max;
            if(max - min +1 > 50000000){  alloc = 50000000; }else{  alloc = max - min +1;}
            bool *distinctCalc = new bool[alloc];
            initTemp(distinctCalc, alloc);

            for (uint64_t k = 0; k < currRelMD->RowsNum; ++k) {
                distinctCalc[currRelMD->RelationSerialData[k + j*currRelMD->RowsNum] - min] = true;
            }
            for (uint64_t l = 0; l < alloc; ++l) {
                if(distinctCalc[l]){
                    distinct = distinct + 1;}
                }
            currRelMD->statistics[j].distinctA = distinct;
            max = 0; min = -1;
            distinct = 0;
            j++;
            delete [] distinctCalc;
            //reinit max min increase j
        }


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
