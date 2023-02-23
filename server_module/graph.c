#include "graph.h"



void setScheme(struct GraphDB *db, struct Column *scheme, int schemeCount) {
    db->scheme = scheme;
    db->schemeCount = schemeCount;
}

void printHeaderGraphDB(struct GraphDB *db) {

    printf("Column count: %d\n", db->schemeCount);

    printf("Scheme:\n");
    int k;
    for (k = 0; k < db->schemeCount; k++) {
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

    printf("Number of rows: %d\n", db->rowCount);
}

void printRow(struct GraphDB *db, struct Row *row) {

    int i;
    for (i = 0; i < db->schemeCount; i++) {
        switch (row->columns[i].type) {
            case INT32:
                printf("%d ", row->columns[i].int32Value);
                break;
            case FLOAT:
                printf("%.2f ", row->columns[i].floatValue);
                break;
            case CHAR_PTR:
                printf("%s ", row->columns[i].charValue);
                break;
            case BOOL:
                printf("%s ", row->columns[i].boolValue ? "true" : "false");
                break;
        }
    }

    printf("\n");
}

void saveHeaderStructToFile(struct GraphDB *db, const char *fileName) {
    FILE *file = fopen(fileName, "wb");

    if (file == NULL) {
        printf("Error opening file (probably it not exists'): %s\n", fileName);
        return;
    }

    fwrite(&db->schemeCount, sizeof(int), 1, file);

    int k;
    for (k = 0; k < db->schemeCount; k++) {
        fwrite(&db->scheme[k], sizeof(struct Column), 1, file);
    }

    fwrite(&db->rowCount, sizeof(int), 1, file);

    fclose(file);
}

void loadHeaderStructFromFile(struct GraphDB *db, const char *fileName) {

    FILE *file = fopen(fileName, "rb");

//    printf("current offset: %d\n", ftell(file));

    if (file == NULL) {
        printf("Error opening file (probably it not exists'): %s\n", fileName);
        return;
    }

    // Read the number of columns in the scheme
    int schemeCount;
    fread(&schemeCount, sizeof(int), 1, file);

//    printf("current offset: %d\n", ftell(file));

    // Read the data type of each column in the scheme
    db->scheme = malloc(schemeCount * sizeof(struct Column));
    db->schemeCount = schemeCount;
    int k;
    for (k = 0; k < schemeCount; k++) {
        fread(&db->scheme[k], sizeof(struct Column), 1, file);
//        printf("current offset: %d\n", ftell(file));
    }

    int rowCount;
    fread(&rowCount, sizeof(int), 1, file);

    db->rowCount = rowCount;

    fclose(file);
}

void movePointerToEndOfHeader(FILE *file) {

    // count of colums
    int schemeCount;
    fread(&schemeCount, sizeof(int), 1, file);

    int k;
    for (k = 0; k < schemeCount; k++) {
        fseek(file, sizeof(struct Column), SEEK_CUR);
    }

    // count of rows
    fseek(file, sizeof(int), SEEK_CUR);
}

// предварительно надо использовать функцию parseAndSetRow
void addRowToFile(const char *fileName, struct Row *row) {

    // нужно для количества columns и rows
    struct GraphDB db;
    loadHeaderStructFromFile(&db, fileName);

//	printf("rowCount: %d \n", db.rowCount);

    // записываем новое количество rows
    FILE *write_file = fopen(fileName, "rb+");
    movePointerToEndOfHeader(write_file);
    fseek(write_file, -sizeof(int), SEEK_CUR);

    int newRowsCount = db.rowCount + 1;
    fwrite(&newRowsCount, sizeof(int), 1, write_file);

//	printf("newRowsCount: %d\n", newRowsCount);
//	printf("current offset: %d\n", ftell(write_file));

    // записываем новую структуру
    fseek(write_file, 0, SEEK_END);

//	printf("current offset: %d\n", ftell(write_file));

    int i;
    for (i = 0; i < db.schemeCount; i++) {
        fwrite(&row->columns[i], sizeof(struct Column), 1, write_file);
    }

//    printf("current offset: %d\n", ftell(write_file));

    fclose(write_file);
}

// file уже должен быть смещен к нужной row
void setRowFromFile(FILE *file, struct Row *row, const int columnsCount) {

    row->columns = malloc(columnsCount * sizeof(struct Column));

    int i;
    for (i = 0; i < columnsCount; i++) {
        fread(&row->columns[i], sizeof(struct Column), 1, file);
    }
}

void iterateByEachRow(const char *fileName) {

    struct GraphDB db;
    loadHeaderStructFromFile(&db, fileName);

    if (db.rowCount == 0) {
        printf("Error: db is empty");
        return;
    }

    FILE *file = fopen(fileName, "rb");
    movePointerToEndOfHeader(file);

    int i;
    for (i = 0; i < db.rowCount; i++) {
        printf("%d. ", i + 1);
        struct Row row;
        setRowFromFile(file, &row, db.schemeCount);
        printRow(&db, &row);

        free(row.columns);
    }

    fclose(file);
}

size_t getFileSize(const char *fileName) {
    FILE *file = fopen(fileName, "rb");

    if (file == NULL) {
        printf("Error opening file (probably it not exists'): %s\n", fileName);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fclose(file);

    return size;
}

bool parseAndSetRow(struct GraphDB *db, char *inputString, struct Row *setted_row) {
    // Create a copy of the input string to tokenize it
    char *inputStringCopy = strdup(inputString);

    // Tokenize the input string using the comma as the delimiter
    char *token = strtok(inputStringCopy, ",");
    int columnIndex = 0;
    struct Row row;
    row.columnCount = db->schemeCount;
    row.columns = malloc(row.columnCount * sizeof(struct Column));
    while (token != NULL && columnIndex < db->schemeCount) {
        struct Column *column = &row.columns[columnIndex];
        strncpy(column->name, db->scheme[columnIndex].name, MAX_NAME_LENGTH);
        column->type = db->scheme[columnIndex].type;

        switch (column->type) {
            case INT32:
                if (sscanf(token, "%d", &column->int32Value) != 1) {
                    free(inputStringCopy);
                    printf("Error add operation, argument %d has ivalid type (needs INT32)\n", columnIndex);
                    return false;
                }
                break;
            case FLOAT:
                if (sscanf(token, "%f", &column->floatValue) != 1) {
                    free(inputStringCopy);
                    printf("Error add operation, argument %d has ivalid type (needs FLOAT)\n", columnIndex);
                    return false;
                }
                break;
            case CHAR_PTR:
                if (strlen(token) + 1 > MAX_CHAR_VALUE) {
                    printf("Error add operation, argument %d is too long (CHAR_PTR)\n", columnIndex);
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
                    printf("Error add operation, argument %d has ivalid type (needs BOOL)\n", columnIndex);
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
    if (columnIndex != db->schemeCount) {
        free(inputStringCopy);
        return false;
    }

    *setted_row = row;

    free(inputStringCopy);

    return true;
}

void clearFileData(const char *fileName) {

    FILE *file = fopen(fileName, "r+");

    if (file == NULL) {
        printf("Error opening file (probably it not exists'): %s\n", fileName);
    }

    ftruncate(fileno(file), 0);
    fclose(file);
}

// =========================== CRUD ================================
// =========================== CRUD ================================

// ======= CREATE ==========

bool createRow(const char *fileName, char *inputString) {

    struct GraphDB db;
    loadHeaderStructFromFile(&db, fileName);

    struct Row newRow;
    bool parseResult = parseAndSetRow(&db, inputString, &newRow);

    if (!parseResult) {
        printf("CreateRow Error: invalid inputString\n");
        return false;
    }

    addRowToFile(fileName, &newRow);

    free(newRow.columns);

    return true;
}

// ======= READ ============

bool findRowById(const char *fileName, int index, struct Row *row) {

    struct GraphDB db;
    loadHeaderStructFromFile(&db, fileName);

    if (index < 0) {
        printf("FIND Error: Invalid index\n");
        return false;
    } else if (db.rowCount == 0) {
        printf("FIND Error: db is empty\n");
        return false;
    } else if (db.rowCount - 1 < index) {
        printf("FIND Error: Out if range\n");
        return false;
    }

    FILE *file = fopen(fileName, "rb");

    movePointerToEndOfHeader(file);

    // смещаемся к нужной row
    int offset = (int) sizeof(struct Column) * db.schemeCount * index;
    fseek(file, offset, SEEK_CUR);

    // читаем row
    setRowFromFile(file, row, db.schemeCount);

//    printRow(&db, row);

    fclose(file);
    return true;
}

size_t findRowsByFilters(const char *fileName, View view, int** result) {
    struct GraphDB db;
    loadHeaderStructFromFile(&db, fileName);

    int found_tmp[db.rowCount];
    int* found_res = malloc(db.rowCount * sizeof(int));

    size_t index = 0;
    size_t counter;

    bool res = false;
    for (int lvl = 0; lvl < view.tree_count; lvl++) {
        if (lvl == 0)
            counter = db.rowCount;
        else
            counter = index;

        index = 0;

        for (int node_idx = 0; node_idx < counter; node_idx++) {
            struct Row row;
            if (lvl == 0)
                findRowById(fileName, node_idx, &row);
            else
                findRowById(fileName, found_res[node_idx], &row);

            for (int and_count = 0; and_count < view.tree[lvl].filters_count; and_count++) {
                for (int or_count = 0; or_count < view.tree[lvl].filters[and_count].conditions_count; or_count++) {
                    res = checkCondition(row, db, view.tree[lvl].filters[and_count].conditions[or_count]);
                    if (res) break;
                }

                if (!res == !view.tree[lvl].filters[and_count].negative) {
                    res = false;
                    break;
                } else if (and_count == view.tree[lvl].filters_count - 1) {
                    res = true;
                }
            }

            if (res) {
                if (lvl == 0)
                    found_tmp[index++] = node_idx;
                else
                    found_tmp[index++] = found_tmp[node_idx];
            }

        }
        memcpy(found_res, found_tmp, index*sizeof(int));
    }

    *result = found_res;
    return index;
}

bool checkCondition(struct Row row, struct GraphDB db, Condition condition) {

    int columnIndex;
    struct Column column;

    for (columnIndex = 0; columnIndex < db.schemeCount; columnIndex++) {
        column = db.scheme[columnIndex];
        if (strcmp(column.name, condition.field_name) == 0) {
            break;
        }
        if (columnIndex == db.schemeCount - 1) {
            printf("FIND Error: column not found");
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
                    return row.columns[columnIndex].int32Value == condition.val.int_val;
                case OP_GREATER:
                    return row.columns[columnIndex].int32Value > condition.val.int_val;
                case OP_LESS:
                    return row.columns[columnIndex].int32Value < condition.val.int_val;
                case OP_NOT_GREATER:
                    return row.columns[columnIndex].int32Value <= condition.val.int_val;
                case OP_NOT_LESS:
                    return row.columns[columnIndex].int32Value >= condition.val.int_val;
            }

        case FLOAT:

            if (!condition.val.has_real_val) {
                printf("FIND Error: argument is not FLOAT");
                return false;
            }

            switch (condition.op) {
                case OP_EQUAL:
                    return row.columns[columnIndex].floatValue == condition.val.real_val;
                case OP_GREATER:
                    return row.columns[columnIndex].floatValue > condition.val.real_val;
                case OP_LESS:
                    return row.columns[columnIndex].floatValue < condition.val.real_val;
                case OP_NOT_GREATER:
                    return row.columns[columnIndex].floatValue <= condition.val.real_val;
                case OP_NOT_LESS:
                    return row.columns[columnIndex].floatValue >= condition.val.real_val;
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

            return strcmp(row.columns[columnIndex].charValue, condition.val.str_val) == 0;
        case BOOL:

            if (!condition.val.has_bool_val) {
                printf("FIND Error: argument is not BOOL");
                return false;
            }
            return row.columns[columnIndex].boolValue == condition.val.bool_val;
    }

}

// ======= UPDATE ==========

void updateRowById(const char *fileName, const char *columnName, const char *columnValue, int index) {

    struct GraphDB db;
    loadHeaderStructFromFile(&db, fileName);

    if (db.rowCount - 1 < index) {
        printf("UPDATE Error: index out of range");
        return;
    }

    if (index < 0) {
        printf("UPDATE Error: Invalid index");
        return;
    }

    int columnIndex;
    for (columnIndex = 0; columnIndex < db.schemeCount; columnIndex++) {

        struct Column column = db.scheme[columnIndex];

        if (strcmp(column.name, columnName) == 0) {
            break;
        } else {
            if (columnIndex == db.schemeCount - 1) {
                printf("UPDATE Error: column not found");
                return;
            }
        }
    }

    FILE *file = fopen(fileName, "rb+");
    movePointerToEndOfHeader(file);

    // смещаемся к нужной row
    int offset = (int) sizeof(struct Column) * db.schemeCount * index;
    fseek(file, offset, SEEK_CUR);

    struct Row row;
    setRowFromFile(file, &row, db.schemeCount);

    // смещаемся обратно к той row, которую будем менять
    fseek(file, -(sizeof(struct Column) * db.schemeCount), SEEK_CUR);

    switch (db.scheme[columnIndex].type) {
        case INT32: {
            int32_t int32Value;

            if (sscanf(columnValue, "%d", &int32Value) != 1) {
                printf("UPDATE Error: argument is not INT32");
                return;
            }

            printf("old int value: %d\n", row.columns[columnIndex].int32Value);

            row.columns[columnIndex].int32Value = int32Value;

            printf("new int value: %d\n", row.columns[columnIndex].int32Value);

            break;
        }
        case FLOAT: {
            float floatValue;

            if (sscanf(columnValue, "%f", &floatValue) != 1) {
                printf("UPDATE Error: argument is not FLOAT");
                return;
            }

            printf("old floatValue value: %f\n", row.columns[columnIndex].floatValue);

            row.columns[columnIndex].floatValue = floatValue;

            printf("new floatValue value: %f\n", row.columns[columnIndex].floatValue);

            break;
        }
        case CHAR_PTR:
            if (strlen(columnValue) + 1 > MAX_CHAR_VALUE) {
                printf("UPDATE Error: string value is too long");
                return;
            }

            printf("old char: ");
            printf(row.columns[columnIndex].charValue);
            printf("\n");

            strncpy(row.columns[columnIndex].charValue, columnValue, MAX_CHAR_VALUE);

            printf("new char: ");
            printf(row.columns[columnIndex].charValue);
            printf("\n");

            break;
        case BOOL: {
            bool boolValue;

            if (strcmp(columnValue, "true") == 0) {
                boolValue = true;
            } else if (strcmp(columnValue, "false") == 0) {
                boolValue = false;
            } else {
                printf("UPDATE Error: argument is not BOOL");
                return;
            }

            printf("old bool value: %d\n", row.columns[columnIndex].boolValue);

            row.columns[columnIndex].boolValue = boolValue;

            printf("new bool value: %d\n", row.columns[columnIndex].boolValue);

            break;
        }
        default:
            return;
    }

    int i;
    for (i = 0; i < db.schemeCount; i++) {
        fwrite(&row.columns[i], sizeof(struct Column), 1, file);
    }

    fclose(file);
}

// ======= DELETE ==========

// =========================== CRUD ================================
// =========================== CRUD ================================






