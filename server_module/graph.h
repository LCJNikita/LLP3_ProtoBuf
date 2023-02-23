#ifndef LLP1_GRAPH_H
#define LLP1_GRAPH_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../proto_module/nanopb/pb.h"
#include "../proto_module/message.pb.h"
#include "../gremlin_module/include/structure.h"

#define MAX_NAME_LENGTH 50
#define MAX_CHAR_VALUE 256


enum DataType {
    INT32,
    FLOAT,
    CHAR_PTR,
    BOOL
};

struct Column {
    char name[MAX_NAME_LENGTH];
    enum DataType type;
    union {
        int32_t int32Value;
        float floatValue;
        char charValue[MAX_CHAR_VALUE];
        bool boolValue;
    };
};

struct Row {
    int columnCount;
    struct Column* columns;
};

struct GraphDB {
    int rowCount;
    int schemeCount;
    struct Column* scheme;
    struct Row* rows;
};

// Utils
void printHeaderGraphDB(struct GraphDB *db);
void printRow(struct GraphDB *db, struct Row *row);
void saveHeaderStructToFile(struct GraphDB* db, const char* fileName);
void loadHeaderStructFromFile(struct GraphDB* db, const char* fileName);
void movePointerToEndOfHeader(FILE* file);
void addRowToFile(const char* fileName, struct Row* row);
void setRowFromFile(FILE* file, struct Row* row, int columnsCount);
void iterateByEachRow(const char* fileName);
size_t getFileSize(const char* fileName);
bool parseAndSetRow(struct GraphDB* db, char* inputString, struct Row* setted_row);
void clearFileData(const char* fileName);

// CRUD
bool createRow(const char* fileName, char* inputString);
bool findRowById(const char *fileName, int index, struct Row* row);
size_t findRowsByFilters(const char *fileName, View view, int** result);
bool checkCondition(struct Row row, struct GraphDB db, Condition condition);
void updateRowById(const char* fileName, const char* columnName, const char* columnValue, int index);
void setScheme(struct GraphDB *db, struct Column* scheme, int schemeCount);

#endif // LLP1_GRAPH_H
