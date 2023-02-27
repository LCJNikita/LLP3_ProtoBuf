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
#include "string_tools.h"

#define MAX_NAME_LENGTH 50
#define MAX_CHAR_VALUE 256
#define MAX_NODE_RELATIONS 50

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

struct Node {
    struct Column* columns;
    int relationsCount;
    int relations[MAX_NODE_RELATIONS];
};

struct GraphDB {
    int nodesCount;
    int columnsCount;
    struct Column* scheme;
    struct Node* nodes;
};

// Utils
void printHeaderGraphDB(struct GraphDB *db);
void printNode(struct GraphDB *db, struct Node *node);
void printNodeToVar(struct GraphDB db, struct Node node, char** str);
void saveHeaderStructToFile(struct GraphDB* db, FILE* file);
void loadHeaderStructFromFile(struct GraphDB *db, FILE* file);
FILE* tryOpenFile(const char *filename);
void movePointerToEndOfHeader(FILE* file);
void addNodeToFile(FILE* file, struct Node *node);
bool parseAndSetNode(struct GraphDB *db, char *inputString, struct Node *setted_node, char** response);
void clearFileData(FILE* file);

// CRUD
void setScheme(struct GraphDB* db, struct Column *scheme, int columnsCount);
void findNodeByIndex(FILE* file, int index, struct Node* node);
bool checkCondition(struct Node node, int id, struct GraphDB db, Condition condition);
size_t findNodesByFilters(FILE* file, View view, int** result);
void updateNodeByIndex(FILE* file, const char* columnName, const char* columnValue, int index);

void deleteNodeByIndex(FILE* file, int index);
void setNewRelation(FILE* file, int index1, int index2);
void clearAllRelationsOfNode(FILE* file, int index);


#endif // LLP1_GRAPH_H
