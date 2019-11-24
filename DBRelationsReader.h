#ifndef TEMPPROJ_DBRELATIONSREADER_H
#define TEMPPROJ_DBRELATIONSREADER_H
#include <iostream>
#include <fstream>

typedef struct RelationMetadata{
    uint64_t TuplesNum,
            RowsNum,
            *RelationSerialData;
}RelationMD;
typedef struct GlobalFilesData{
    int relationNum;
    RelationMD * relationsData;
}Data;
void printDefinedStruct(RelationMD * relationMd);
void DeleteData(Data * data);
int getInitFileLines(const std::string& fileName);
int getRelationData(const std::string& fileName, RelationMD * currRelMD);
Data * getData(const std::string& fileName);
#endif //TEMPPROJ_DBRELATIONSREADER_H
