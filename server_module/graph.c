#include "graph.h"


void setScheme(struct GraphDB *db, struct Column *scheme, int columnsCount) {
    db->scheme = scheme;
    db->columnsCount = columnsCount;
}

void printHeaderGraphDB(struct GraphDB *db) {

    printf("Column count: %d\n", db->columnsCount);

    printf("Scheme:\n");
    int k;
    for (k = 0; k < db->columnsCount; k++) {
        printf("\t%d. %s", k + 1, db->scheme[k].name);

        switch (db->scheme[k].type) {
            case INT32:
                printf(" (INT32)\n");
                break;
            case FLOAT:
                printf(" (FLOAT)\n");
                break;
            case CHAR_PTR:
                printf(" (CHAR_PTR)\n");
                break;
            case BOOL:
                printf(" (BOOL)\n");
                break;
        }
    }

    printf("Number of nodes: %d\n", db->nodesCount);
}

void printNode(struct GraphDB *db, struct Node *node) {

    int i;
    for (i = 0; i < db->columnsCount; i++) {
        switch (node->columns[i].type) {
            case INT32:
                printf("%d ", node->columns[i].int32Value);
                break;
            case FLOAT:
                printf("%.2f ", node->columns[i].floatValue);
                break;
            case CHAR_PTR:
                printf("%s ", node->columns[i].charValue);
                break;
            case BOOL:
                printf("%s ", node->columns[i].boolValue ? "true" : "false");
                break;
        }
    }

    printf("| relations number (%d) ", node->relationsCount);
    printf("relations: [");

    if (node->relationsCount != 0) {
        int k;
        for (k = 0; k < MAX_NODE_RELATIONS; k++) {
            if (node->relations[k] != -1) {
                printf(" %d ", node->relations[k]);
            }
        }
    }
    printf("] \n");
}

void printNodeToVar(struct GraphDB db, struct Node node, char **str) {
    char *tmp = malloc(sizeof(char) * STR_LEN);

    for (size_t i = 0; i < db.columnsCount; i++) {
        sprintf(tmp, "%s: ", node.columns[i].name);
        *str = concat(*str, tmp);

        switch (node.columns[i].type) {
            case INT32:
                sprintf(tmp, "%d\n", node.columns[i].int32Value);
                break;
            case FLOAT:
                sprintf(tmp, "%.2f\n", node.columns[i].floatValue);
                break;
            case CHAR_PTR:
                sprintf(tmp, "%s\n", node.columns[i].charValue);
                break;
            case BOOL:
                sprintf(tmp, "%s\n", node.columns[i].boolValue ? "true" : "false");
                break;
        }
        *str = concat(*str, tmp);
    }

    sprintf(tmp, "\nRelations count: %d\nRelations: [", node.relationsCount);
    *str = concat(*str, tmp);

    if (node.relationsCount != 0) {
        for (size_t k = 0; k < MAX_NODE_RELATIONS; k++) {
            if (node.relations[k] != -1) {
                sprintf(tmp, " %d ", node.relations[k]);
                *str = concat(*str, tmp);
            }
        }
    }
    *str = concat(*str, "]\n\n");
    free(tmp);
}

void saveHeaderStructToFile(struct GraphDB *db, FILE* file) {
    long saved = ftell(file);

    fseek(file, 0, SEEK_SET);
    fwrite(&db->columnsCount, sizeof(int), 1, file);

    int k;
    for (k = 0; k < db->columnsCount; k++) {
        fwrite(&db->scheme[k], sizeof(struct Column), 1, file);
    }

    fwrite(&db->nodesCount, sizeof(int), 1, file);

    fseek(file, saved, SEEK_SET);
}

void loadHeaderStructFromFile(struct GraphDB *db, FILE* file) {
    long saved = ftell(file);


    fseek(file, 0, SEEK_SET);
    // Read the number of columns in the scheme
    int columnsCount;
    fread(&columnsCount, sizeof(int), 1, file);

    // Read the data type of each column in the scheme
    db->scheme = malloc(columnsCount * sizeof(struct Column));
    db->columnsCount = columnsCount;
    int k;
    for (k = 0; k < columnsCount; k++) {
        fread(&db->scheme[k], sizeof(struct Column), 1, file);
    }

    int nodesCount;
    fread(&nodesCount, sizeof(int), 1, file);
    db->nodesCount = nodesCount;

    fseek(file, saved, SEEK_SET);
}

FILE* tryOpenFile(const char *filename){
    FILE *file = fopen(filename, "rb+");

    if (file == NULL) {
        printf("File doesn't exist. Creating new with default scheme...\n");
        file = fopen(filename, "w+b");
        int columnsCount = 4;
        struct Column scheme[columnsCount];
        scheme[0].type = INT32;
        strncpy(scheme[0].name, "Age\0", MAX_NAME_LENGTH);
        scheme[1].type = FLOAT;
        strncpy(scheme[1].name, "Balance\0", MAX_NAME_LENGTH);
        scheme[2].type = CHAR_PTR;
        strncpy(scheme[2].name, "Name\0", MAX_NAME_LENGTH);
        scheme[3].type = BOOL;
        strncpy(scheme[3].name, "isAdult\0", MAX_NAME_LENGTH);
        struct GraphDB db = {0};
        setScheme(&db, scheme, columnsCount);
        saveHeaderStructToFile(&db, file);
    }

    return file;
}

void movePointerToEndOfHeader(FILE *file) {
    fseek(file, 0, SEEK_SET);

    // count of colums
    int columnsCount;
    fread(&columnsCount, sizeof(int), 1, file);

    int k;
    for (k = 0; k < columnsCount; k++) {
        fseek(file, sizeof(struct Column), SEEK_CUR);
    }

    // count of rows
    fseek(file, sizeof(int), SEEK_CUR);
}


// before use parseAndSetRow to create a row
void addNodeToFile(FILE* file, struct Node *node) {
    long saved = ftell(file);

    // for columns and rows count
    struct GraphDB db;
    loadHeaderStructFromFile(&db, file);

    // write new count of rows
    movePointerToEndOfHeader(file);
    fseek(file, -sizeof(int), SEEK_CUR);

    int newNodesCount = db.nodesCount + 1;
    fwrite(&newNodesCount, sizeof(int), 1, file);


    // write new row
    fseek(file, 0, SEEK_END);


    int i;
    for (i = 0; i < db.columnsCount; i++) {
        fwrite(&node->columns[i], sizeof(struct Column), 1, file);
    }

    int relationsCount = 0;
    fwrite(&relationsCount, sizeof(int), 1, file);

    int k;
    for (k = 0; k < MAX_NODE_RELATIONS; k++) {
        int relationValue = -1;
        fwrite(&relationValue, sizeof(int), 1, file);
    }

    fseek(file, saved, SEEK_SET);
}

// file needs offset to the row before calling
void setNodeFromFile(FILE *file, struct Node *node, const int columnsCount) {
    long saved = ftell(file);

    node->columns = malloc(columnsCount * sizeof(struct Column));

    int i;
    for (i = 0; i < columnsCount; i++) {
        fread(&node->columns[i], sizeof(struct Column), 1, file);
    }

    fread(&node->relationsCount, sizeof(int), 1, file);

    int k;
    for (k = 0; k < MAX_NODE_RELATIONS; k++) {
        fread(&node->relations[k], sizeof(int), 1, file);
    }

    fseek(file, saved, SEEK_SET);
}

// file needs offset to the row before calling
void writeNodeToFile(FILE *file, struct Node *node, const int columnsCount) {
    long saved = ftell(file);

    for (int i = 0; i < columnsCount; i++) {
        fwrite(&node->columns[i], sizeof(struct Column), 1, file);
    }

    fwrite(&node->relationsCount, sizeof(int), 1, file);

    int k;
    for (k = 0; k < MAX_NODE_RELATIONS; k++) {
        fwrite(&node->relations[k], sizeof(int), 1, file);
    }


    fseek(file, saved, SEEK_SET);
}

bool parseAndSetNode(struct GraphDB *db, char *inputString, struct Node *setted_node, char** response) {
    // Create a copy of the input string to tokenize it
    char *inputStringCopy = strdup(inputString);

    // Tokenize the input string using the comma as the delimiter
    char *token = strtok(inputStringCopy, ",");
    char* tmp;
    int columnIndex = 0;
    struct Node node;
    node.columns = malloc(db->columnsCount * sizeof(struct Column));
    while (token != NULL && columnIndex < db->columnsCount) {
        struct Column *column = &node.columns[columnIndex];
        strncpy(column->name, db->scheme[columnIndex].name, MAX_NAME_LENGTH);
        column->type = db->scheme[columnIndex].type;

        switch (column->type) {
            case INT32:
                if (sscanf(token, "%d", &column->int32Value) != 1) {
                    free(inputStringCopy);
                    sprintf(tmp, "Error while adding!\nArgument %d has invalid type (INT32 expected).\n", columnIndex);
                    *response = concat(*response, tmp);
                    return false;
                }
                break;
            case FLOAT:
                if (sscanf(token, "%f", &column->floatValue) != 1) {
                    free(inputStringCopy);
                    sprintf(tmp, "Error while adding!\nArgument %d has invalid type (FLOAT expected).\n", columnIndex);
                    *response = concat(*response, tmp);
                    return false;
                }
                break;
            case CHAR_PTR:
                if (strlen(token) + 1 > MAX_CHAR_VALUE) {
                    free(inputStringCopy);
                    sprintf(tmp, "Error while adding!\nArgument %d is too long (CHAR_PTR).\n", columnIndex);
                    *response = concat(*response, tmp);
                    return false;
                }
                strcpy(column->charValue, token);
                break;
            case BOOL:
                if (strcmp(token, "true") == 0) {
                    column->boolValue = true;
                } else if (strcmp(token, "false") == 0) {
                    column->boolValue = false;
                } else {
                    free(inputStringCopy);
                    sprintf(tmp, "Error while adding!\nArgument %d has invalid type (BOOL expected).\n", columnIndex);
                    *response = concat(*response, tmp);
                    return false;
                }
                break;
            default:
                break;
        }

        token = strtok(NULL, ",");
        columnIndex++;
    }

    // Check if all the columns were parsed
    if (columnIndex != db->columnsCount) {
        free(inputStringCopy);
        return false;
    }
    node.relationsCount = 0;
    *setted_node = node;

    free(inputStringCopy);

    return true;
}

void clearFileData(FILE* file) {
    ftruncate(fileno(file), 0);
}

// =========================== CRUD ================================


// ======= READ ============

void findNodeByIndex(FILE* file, int index, struct Node *node) {
    long saved = ftell(file);

    struct GraphDB db;
    loadHeaderStructFromFile(&db, file);

    if (index < 0) {
        printf("FIND Error: Invalid index\n");
        fseek(file, saved, SEEK_SET);
        return;
    } else if (db.nodesCount == 0) {
        printf("FIND Error: db is empty\n");
        fseek(file, saved, SEEK_SET);
        return;
    } else if (db.nodesCount - 1 < index) {
        printf("FIND Error: Out if range\n");
        fseek(file, saved, SEEK_SET);
        return;
    }

    movePointerToEndOfHeader(file);

    int offset = ((int) sizeof(struct Column) * db.columnsCount + (int) sizeof(int) +
                  (int) sizeof(int) * MAX_NODE_RELATIONS) * index;
    fseek(file, offset, SEEK_CUR);

    setNodeFromFile(file, node, db.columnsCount);

    fseek(file, saved, SEEK_SET);
}

size_t findNodesByFilters(FILE* file, View view, int **result) {
    long saved = ftell(file);

    struct GraphDB db;
    loadHeaderStructFromFile(&db, file);

    int found_tmp[db.nodesCount];
    int *found_res = malloc(db.nodesCount * sizeof(int));

    size_t index = 0;
    size_t counter;

    bool res = false;
    for (int lvl = (int) view.tree_count - 1; lvl >= 0; --lvl) {
        if (lvl == view.tree_count - 1)
            counter = db.nodesCount;
        else
            counter = index;

        index = 0;

        for (int iter = 0; iter < counter; iter++) {
            struct Node node;
            int node_index = lvl == view.tree_count - 1 ? iter : found_res[iter];

            findNodeByIndex(file, node_index, &node);

            if (view.tree[lvl].filters_count == 0) {
                res = true;
            } else {
                for (int and_count = 0; and_count < view.tree[lvl].filters_count; and_count++) {
                    for (int or_count = 0; or_count < view.tree[lvl].filters[and_count].conditions_count; or_count++) {
                        res = checkCondition(node, node_index, db,
                                             view.tree[lvl].filters[and_count].conditions[or_count]);
                        if (res) break;
                    }

                    if (!res == !view.tree[lvl].filters[and_count].negative) {
                        res = false;
                        break;
                    } else if (and_count == view.tree[lvl].filters_count - 1) {
                        res = true;
                    }
                }
            }

            if (res) {
                if (lvl == view.tree_count - 1)
                    found_tmp[index++] = iter;
                else
                    found_tmp[index++] = found_res[iter];
            }

        }
        if (lvl == 0) {
            memcpy(found_res, found_tmp, index * sizeof(int));
        } else {
            struct Node node_tmp;
            memset(found_res, -1, db.nodesCount * sizeof(*found_res));
            size_t index_tmp = 0;

            for (size_t i = 0; i < index; i++) {
                findNodeByIndex(file, found_tmp[i], &node_tmp);
                for (int rel_iter = 0; rel_iter < node_tmp.relationsCount; rel_iter++) {
                    bool duplicate = false;
                    for (int j = 0; j < index_tmp; j++) {
                        if (found_res[j] == node_tmp.relations[rel_iter]) {
                            duplicate = true;
                            break;
                        }
                    }
                    if (!duplicate) {
                        found_res[index_tmp++] = node_tmp.relations[rel_iter];
                    }

                }
            }
            index = index_tmp;
        }

    }

    *result = found_res;
    fseek(file, saved, SEEK_SET);
    return index;
}


bool checkCondition(struct Node node, int id, struct GraphDB db, Condition condition) {

    if (condition.is_id) {
        return condition.id == id;
    } else {
        int columnIndex;
        struct Column column;
        for (columnIndex = 0; columnIndex < db.columnsCount; columnIndex++) {
            column = db.scheme[columnIndex];
            if (strcmp(column.name, condition.field_name) == 0) {
                break;
            }
            if (columnIndex == db.columnsCount - 1) {
                printf("FIND Error: column not found\n");
                return false;
            }
        }

        switch (column.type) {
            case INT32:
                if (!condition.val.has_int_val) {
                    printf("FIND Error: argument is not INT32");
                    return false;
                }
                switch (condition.op) {
                    case OP_EQUAL:
                        return node.columns[columnIndex].int32Value == condition.val.int_val;
                    case OP_GREATER:
                        return node.columns[columnIndex].int32Value > condition.val.int_val;
                    case OP_LESS:
                        return node.columns[columnIndex].int32Value < condition.val.int_val;
                    case OP_NOT_GREATER:
                        return node.columns[columnIndex].int32Value <= condition.val.int_val;
                    case OP_NOT_LESS:
                        return node.columns[columnIndex].int32Value >= condition.val.int_val;
                }

            case FLOAT:

                if (!condition.val.has_real_val) {
                    printf("FIND Error: argument is not FLOAT");
                    return false;
                }

                switch (condition.op) {
                    case OP_EQUAL:
                        return node.columns[columnIndex].floatValue == condition.val.real_val;
                    case OP_GREATER:
                        return node.columns[columnIndex].floatValue > condition.val.real_val;
                    case OP_LESS:
                        return node.columns[columnIndex].floatValue < condition.val.real_val;
                    case OP_NOT_GREATER:
                        return node.columns[columnIndex].floatValue <= condition.val.real_val;
                    case OP_NOT_LESS:
                        return node.columns[columnIndex].floatValue >= condition.val.real_val;
                }

            case CHAR_PTR:
                if (!condition.val.has_str_val) {
                    printf("FIND Error: argument is not STRING");
                    return false;
                }

                if (strlen(condition.val.str_val) + 1 > MAX_CHAR_VALUE) {
                    printf("FIND Error: string value is too long");
                    return false;
                }

                return strcmp(node.columns[columnIndex].charValue, condition.val.str_val) == 0;
            case BOOL:

                if (!condition.val.has_bool_val) {
                    printf("FIND Error: argument is not BOOL");
                    return false;
                }
                return node.columns[columnIndex].boolValue == condition.val.bool_val;
        }
    }
}

// ======= UPDATE ==========

void updateNodeByIndex(FILE* file, const char *columnName, const char *columnValue, int index) {
    long saved = ftell(file);

    struct GraphDB db;
    loadHeaderStructFromFile(&db, file);

    if (db.nodesCount - 1 < index) {
        printf("UPDATE Error: index out of range");
        fseek(file, saved, SEEK_SET);
        return;
    }

    if (index < 0) {
        printf("UPDATE Error: Invalid index");
        fseek(file, saved, SEEK_SET);
        return;
    }

    int columnIndex;
    for (columnIndex = 0; columnIndex < db.columnsCount; columnIndex++) {

        struct Column column = db.scheme[columnIndex];

        if (strcmp(column.name, columnName) == 0) {
            break;
        } else {
            if (columnIndex == db.columnsCount - 1) {
                printf("UPDATE Error: column not found");
                fseek(file, saved, SEEK_SET);
                return;
            }
        }
    }

    movePointerToEndOfHeader(file);

    int nodeSize =
            (int) sizeof(struct Column) * db.columnsCount + (int) sizeof(int) + (int) sizeof(int) * MAX_NODE_RELATIONS;
    int offset = nodeSize * index;
    fseek(file, offset, SEEK_CUR);

    struct Node node;
    setNodeFromFile(file, &node, db.columnsCount);

    switch (db.scheme[columnIndex].type) {
        case INT32: {
            int32_t int32Value;

            if (sscanf(columnValue, "%d", &int32Value) != 1) {
                printf("UPDATE Error: argument is not INT32");
                fseek(file, saved, SEEK_SET);
                return;
            }

            node.columns[columnIndex].int32Value = int32Value;
            break;
        }
        case FLOAT: {
            float floatValue;

            if (sscanf(columnValue, "%f", &floatValue) != 1) {
                printf("UPDATE Error: argument is not FLOAT");
                fseek(file, saved, SEEK_SET);
                return;
            }
            node.columns[columnIndex].floatValue = floatValue;

            break;
        }
        case CHAR_PTR:
            if (strlen(columnValue) + 1 > MAX_CHAR_VALUE) {
                printf("UPDATE Error: string value is too long");
                fseek(file, saved, SEEK_SET);
                return;
            }

            strncpy(node.columns[columnIndex].charValue, columnValue, MAX_CHAR_VALUE);
            break;
        case BOOL: {
            bool boolValue;

            if (strcmp(columnValue, "true") == 0) {
                boolValue = true;
            } else if (strcmp(columnValue, "false") == 0) {
                boolValue = false;
            } else {
                printf("UPDATE Error: argument is not BOOL");
                fseek(file, saved, SEEK_SET);
                return;
            }

            node.columns[columnIndex].boolValue = boolValue;
            break;
        }
        default:
            fseek(file, saved, SEEK_SET);
            return;
    }

    for (size_t i = 0; i < db.columnsCount; i++) {
        fwrite(&node.columns[i], sizeof(struct Column), 1, file);
    }
    fseek(file, saved, SEEK_SET);

}

// ======= DELETE ==========

void dropRelation(FILE* file, int index1, int index2) {
    long saved = ftell(file);

    struct GraphDB db;
    loadHeaderStructFromFile(&db, file);

    if (index1 < 0 || index2 < 0) {
        printf("DropRelation Error: Invalid index\n");
        return;
    } else if (db.nodesCount == 0) {
        printf("DropRelation Error: db is empty\n");
        return;
    } else if (db.nodesCount - 1 < index1 || db.nodesCount - 1 < index2) {
        printf("DropRelation Error: Out if range\n");
        return;
    } else if (index1 == index2) {
        printf("DropRelation Error: indexes are equal\n");
        return;
    }

    int nodeSize =
            (int) sizeof(struct Column) * db.columnsCount + (int) sizeof(int) + (int) sizeof(int) * MAX_NODE_RELATIONS;

    movePointerToEndOfHeader(file);
    fseek(file, nodeSize * index1, SEEK_CUR);

    struct Node node;
    setNodeFromFile(file, &node, db.columnsCount);

    node.relationsCount = node.relationsCount - 1;


    for (int j = 0; j < MAX_NODE_RELATIONS; j++) {
        if (node.relations[j] == index2) {
            node.relations[j] = -1;
            break;
        }
    }

    writeNodeToFile(file, &node, db.columnsCount);
    fseek(file, saved, SEEK_SET);
}


// index1 - node where update relation
// oldIndex
// newIndex
// function helper, when we delete some nodes
void updateRelation(FILE* file, int index, int oldIndex, int newIndex) {

    struct GraphDB db;
    loadHeaderStructFromFile(&db, file);

    if (index < 0 || oldIndex < 0 || newIndex < 0) {
        printf("DropRelation Error: Invalid index\n");
        return;
    } else if (db.nodesCount == 0) {
        printf("DropRelation Error: db is empty\n");
        return;
    } else if (db.nodesCount - 1 < index || db.nodesCount - 1 < oldIndex || db.nodesCount - 1 < newIndex) {
        printf("DropRelation Error: Out if range\n");
        return;
    } else if (index == oldIndex || index == newIndex || oldIndex == newIndex) {
        printf("DropRelation Error: indexes are equal\n");
        return;
    }

    int nodeSize =
            (int) sizeof(struct Column) * db.columnsCount + (int) sizeof(int) + (int) sizeof(int) * MAX_NODE_RELATIONS;

    movePointerToEndOfHeader(file);
    fseek(file, nodeSize * index, SEEK_CUR);

    struct Node node;
    setNodeFromFile(file, &node, db.columnsCount);

    int j;
    for (j = 0; j < MAX_NODE_RELATIONS; j++) {
        if (node.relations[j] == oldIndex) {
            node.relations[j] = newIndex;
        }
    }

    writeNodeToFile(file, &node, db.columnsCount);
}

void deleteNodeByIndex(FILE* file, int index) {
    long saved = ftell(file);

    struct GraphDB db;
    loadHeaderStructFromFile(&db, file);

    if (index < 0) {
        printf("DELETE Error: Invalid index\n");
        fseek(file, saved, SEEK_SET);
        return;
    } else if (db.nodesCount == 0) {
        printf("DELETE Error: db is empty\n");
        fseek(file, saved, SEEK_SET);
        return;
    } else if (db.nodesCount - 1 < index) {
        printf("DELETE Error: Out if range\n");
        fseek(file, saved, SEEK_SET);
        return;
    }

    movePointerToEndOfHeader(file);

    size_t headerSize = ftell(file);
    int nodeSize =
            (int) sizeof(struct Column) * db.columnsCount + (int) sizeof(int) + (int) sizeof(int) * MAX_NODE_RELATIONS;

    if (db.nodesCount == 1) {
        // remove without replacing (if db contains only 1 element and delete it)
        ftruncate(fileno(file), headerSize);
    } else if (db.nodesCount - 1 == index) {
        // remove without replacing (if delete last element)
        fseek(file, -nodeSize, SEEK_END);
        struct Node lastNode;
        setNodeFromFile(file, &lastNode, db.columnsCount);

        int k;
        for (k = 0; k < MAX_NODE_RELATIONS; k++) {
            if (lastNode.relations[k] != -1) {
                dropRelation(file, lastNode.relations[k], index);
            }
        }

        int newFileSize = headerSize + nodeSize * (db.nodesCount - 1);
        ftruncate(fileno(file), newFileSize);

    } else {
        printf("HERE deleting with replacing \n");

        fseek(file, nodeSize * index, SEEK_CUR);
        struct Node deletingNode;
        setNodeFromFile(file, &deletingNode, db.columnsCount);

        int i;
        for (i = 0; i < MAX_NODE_RELATIONS; i++) {
            if (deletingNode.relations[i] != -1) {
                dropRelation(file, deletingNode.relations[i], index);
            }
        }

//        fclose(file);
//        file = fopen(fileName, "rb+");

        fseek(file, -nodeSize, SEEK_END);
        struct Node lastNode;
        setNodeFromFile(file, &lastNode, db.columnsCount);

        int lastIndex = db.nodesCount - 1;

        int k;
        for (k = 0; k < MAX_NODE_RELATIONS; k++) {
            if (lastNode.relations[k] != -1) {
                updateRelation(file, lastNode.relations[k], lastIndex, index);
            }
        }

        fseek(file, 0, SEEK_SET);
        movePointerToEndOfHeader(file);
        fseek(file, nodeSize * index, SEEK_CUR);
        int j;
        for (j = 0; j < db.columnsCount; j++) {
            fwrite(&lastNode.columns[j], sizeof(struct Column), 1, file);
        }

        int relationsCount = lastNode.relationsCount;
        fwrite(&relationsCount, sizeof(int), 1, file);

        int a;
        for (a = 0; a < MAX_NODE_RELATIONS; a++) {
            int relationValue = lastNode.relations[a];
            fwrite(&relationValue, sizeof(int), 1, file);
        }

        int newFileSize = headerSize + nodeSize * (db.nodesCount - 1);
        ftruncate(fileno(file), newFileSize);
    }

    fseek(file, 0, SEEK_SET);
    movePointerToEndOfHeader(file);
    fseek(file, -sizeof(int), SEEK_CUR);
    int newNodesCount = db.nodesCount - 1;
    fwrite(&newNodesCount, sizeof(int), 1, file);

    fseek(file, saved, SEEK_SET);
}

// ====================== Relations ================================

void setNewRelation(FILE* file, int index1, int index2) {
    long saved = ftell(file);

    struct GraphDB db;
    loadHeaderStructFromFile(&db, file);

    if (index1 < 0 || index2 < 0) {
        printf("NewRelation Error: Invalid index\n");
        fseek(file, saved, SEEK_SET);
        return;
    } else if (db.nodesCount == 0) {
        printf("NewRelation Error: db is empty\n");
        fseek(file, saved, SEEK_SET);
        return;
    } else if (db.nodesCount - 1 < index1 || db.nodesCount - 1 < index2) {
        printf("NewRelation Error: Out if range\n");
        fseek(file, saved, SEEK_SET);
        return;
    } else if (index1 == index2) {
        printf("NewRelation Error: indexes are equal\n");
        fseek(file, saved, SEEK_SET);
        return;
    }

    int nodeSize =
            (int) sizeof(struct Column) * db.columnsCount + (int) sizeof(int) + (int) sizeof(int) * MAX_NODE_RELATIONS;

    movePointerToEndOfHeader(file);
    fseek(file, nodeSize * index1, SEEK_CUR);

    struct Node node1;
    setNodeFromFile(file, &node1, db.columnsCount);

    fseek(file, 0, SEEK_SET);
    movePointerToEndOfHeader(file);
    fseek(file, nodeSize * index2, SEEK_CUR);

    struct Node node2;
    setNodeFromFile(file, &node2, db.columnsCount);


    if (node1.relationsCount == 50) {
        printf("NewRelation Error: node1 has max relations\n");
        fseek(file, saved, SEEK_SET);
        return;
    }

    if (node2.relationsCount == 50) {
        printf("NewRelation Error: node2 has max relations\n");
        fseek(file, saved, SEEK_SET);
        return;
    }

    int k;
    for (k = 0; k < MAX_NODE_RELATIONS; k++) {
        if (node1.relations[k] == index2) {
            printf("NewRelation warning: relation already exists\n");
            fseek(file, saved, SEEK_SET);
            return;
        }
    }
    int i;
    for (i = 0; i < MAX_NODE_RELATIONS; i++) {
        if (node1.relations[i] == -1) {
            node1.relations[i] = index2;
            break;
        }
    }

    node1.relationsCount = node1.relationsCount + 1;

    fseek(file, 0, SEEK_SET);
    movePointerToEndOfHeader(file);
    fseek(file, nodeSize * index1, SEEK_CUR);

    writeNodeToFile(file, &node1, db.columnsCount);

    int j;
    for (j = 0; j < MAX_NODE_RELATIONS; j++) {
        if (node2.relations[j] == -1) {
            node2.relations[j] = index1;
            break;
        }
    }

    node2.relationsCount = node2.relationsCount + 1;

    fseek(file, 0, SEEK_SET);
    movePointerToEndOfHeader(file);
    fseek(file, nodeSize * index2, SEEK_CUR);

    writeNodeToFile(file, &node2, db.columnsCount);

    fseek(file, saved, SEEK_SET);
}

// index1 - node where drop relation
// index2 - drop index
// function helper, when we update relations or delete some nodes

void clearAllRelationsOfNode(FILE* file, int index) {
    long saved = ftell(file);

    struct GraphDB db;
    loadHeaderStructFromFile(&db, file);

    if (index < 0) {
        printf("ClearRelation Error: Invalid index\n");
        fseek(file, saved, SEEK_SET);
        return;
    } else if (db.nodesCount == 0) {
        printf("ClearRelation Error: db is empty\n");
        fseek(file, saved, SEEK_SET);
        return;
    } else if (db.nodesCount - 1 < index) {
        printf("ClearRelation Error: Out if range\n");
        fseek(file, saved, SEEK_SET);
        return;
    }

    int nodeSize =
            (int) sizeof(struct Column) * db.columnsCount + (int) sizeof(int) + (int) sizeof(int) * MAX_NODE_RELATIONS;

    movePointerToEndOfHeader(file);
    fseek(file, nodeSize * index, SEEK_CUR);

    struct Node node;
    setNodeFromFile(file, &node, db.columnsCount);
    if (node.relationsCount == 0) {
        printf("ClearRelation: node has zero relations");
        fseek(file, saved, SEEK_SET);
        return;
    }

    // drop relations in other nodes
    int k;
    for (k = 0; k < MAX_NODE_RELATIONS; k++) {
        if (node.relations[k] != -1) {
            dropRelation(file, node.relations[k], index);
        }
    }

    // update current node with zero relations
    node.relationsCount = 0;
    int j;
    for (j = 0; j < MAX_NODE_RELATIONS; j++) {
        node.relations[j] = -1;
    }

    writeNodeToFile(file, &node, db.columnsCount);
    fseek(file, saved, SEEK_SET);
}

