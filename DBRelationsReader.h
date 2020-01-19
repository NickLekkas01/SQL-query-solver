#ifndef TEMPPROJ_DBRELATIONSREADER_H
#define TEMPPROJ_DBRELATIONSREADER_H
#include <iostream>
#include <fstream>

typedef struct RelationColumnStats{
    uint64_t upperA=0, lowerA=0,fA =0,distinctA=0;
    bool changed=false;
}RelationCS;
typedef struct QueryStatistics{
    uint8_t numOfBindings;
    uint8_t * TuplesPerBinding;
    RelationCS ** stats;
}QueryStats;
typedef struct RelationMetadata{
    uint8_t TuplesNum;
    uint64_t RowsNum,
            *RelationSerialData;
    std::string name;
    RelationCS * statistics;
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
