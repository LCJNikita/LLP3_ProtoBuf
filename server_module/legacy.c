#include "graph.h"
#include <stdlib.h>

void addRow(struct GraphDB* db, struct Row row) {
    db->rows = realloc(db->rows, (db->rowCount + 1) * sizeof(struct Row));
    db->rows[db->rowCount++] = row;
}

void freeGraphDB(struct GraphDB *db) {
    int k;
    for (k = 0; k < db->schemeCount; k++) {
        free(db->scheme[k].name);
    }
    free(db->scheme);
    db->scheme = NULL;
    db->schemeCount = 0;

    int i;
    for (i = 0; i < db->rowCount; i++) {
        free(db->rows[i].columns);
    }
    free(db->rows);
    db->rows = NULL;
    db->rowCount = 0;
}

void saveToFile(struct GraphDB* db, const char* fileName) {
    FILE* file = fopen(fileName, "wb");
    
    if (file == NULL) {
    	printf("Error opening file (probably it not exists'): %s\n", fileName);
    	return;
	}

    fwrite(&db->schemeCount, sizeof(int), 1, file);

    int k;
    for (k = 0; k < db->schemeCount; k++) {
        fwrite(&db->scheme[k].type, sizeof(enum DataType), 1, file);
        fwrite(&db->scheme[k].name, (size_t)MAX_NAME_LENGTH, 1, file);
    }

    fwrite(&db->rowCount, sizeof(int), 1, file);

    int i;
    for (i = 0; i < db->rowCount; i++) {
        struct Row row = db->rows[i];

        fwrite(&row.columnCount, sizeof(int), 1, file);

        int j;
        for (j = 0; j < row.columnCount; j++) {
            struct Column column = row.columns[j];

            fwrite(&column.type, sizeof(enum DataType), 1, file);

            switch (column.type) {
                case INT32:
                    fwrite(&column.int32Value, sizeof(int32_t), 1, file);
                    break;
                case FLOAT:
                    fwrite(&column.floatValue, sizeof(float), 1, file);
                    break;
                case CHAR_PTR:
                    {
						fwrite(column.charValue, sizeof(MAX_CHAR_VALUE), 1, file);
                        break;
                    }
                case BOOL:
                    fwrite(&column.boolValue, sizeof(bool), 1, file);
                    break;
                default:
                    break;
            }
        }
    }

    fclose(file);
}

void printGraphDB(struct GraphDB *db) {
    // Print the scheme
//    printHeaderGraphDB(db);

    // Print the rows
    printf("Rows:\n");
    int i;
    for (i = 0; i < db->rowCount; i++) {
        printf("\t%d. ", i + 1);
        int j;
        for (j = 0; j < db->rows[i].columnCount; j++) {
            switch (db->rows[i].columns[j].type) {
                case INT32:
                    printf("%d ", db->rows[i].columns[j].int32Value);
                    break;
                case FLOAT:
                    printf("%.2f ", db->rows[i].columns[j].floatValue);
                    break;
                case CHAR_PTR:
                    printf("%s ", db->rows[i].columns[j].charValue);
                    break;
                case BOOL:
                	printf("%s ", db->rows[i].columns[j].boolValue ? "true" : "false");
                    break;
            }
        }
        printf("\n");
    }
}

void saveHeaderToFile(struct GraphDB* db, const char* fileName) {
    FILE* file = fopen(fileName, "wb");
    
    if (file == NULL) {
        printf("Error opening file (probably it not exists'): %s\n", fileName);
        return;
    }

    // Write the number of columns in the scheme
    fwrite(&db->schemeCount, sizeof(int), 1, file);
    
//    printf("sizeof(int): %d\n", (int)sizeof(int));

    // Write the data type of each column in the scheme
    int k;
    for (k = 0; k < db->schemeCount; k++) {
        fwrite(&db->scheme[k].type, sizeof(enum DataType), 1, file);
        fwrite(&db->scheme[k].name, (size_t)MAX_NAME_LENGTH, 1, file);
    }
    
    fwrite(&db->rowCount, sizeof(int), 1, file);

    fclose(file);
}

void loadFromFile(struct GraphDB* db, const char* fileName) {
    FILE* file = fopen(fileName, "rb");
    
    if (file == NULL) {
    	printf("Error opening file (probably it not exists'): %s\n", fileName);
    	return;
	}

    int schemeCount;
    fread(&schemeCount, sizeof(int), 1, file);

    db->scheme = malloc(schemeCount * sizeof(struct Column));
    db->schemeCount = schemeCount;
    int k;
    for (k = 0; k < schemeCount; k++) {
        fread(&db->scheme[k].type, sizeof(enum DataType), 1, file);
        fread(&db->scheme[k].name, (size_t)MAX_NAME_LENGTH, 1, file);
    }

    int rowCount;
    fread(&rowCount, sizeof(int), 1, file);

    db->rows = malloc(rowCount * sizeof(struct Row));
    db->rowCount = rowCount;
    int i;
    for (i = 0; i < rowCount; i++) {
        struct Row* row = &db->rows[i];

        int columnCount;
        fread(&columnCount, sizeof(int), 1, file);

        row->columns = malloc(columnCount * sizeof(struct Column));
        row->columnCount = columnCount;
        int j;
        for (j = 0; j < columnCount; j++) {
            struct Column* column = &row->columns[j];

            fread(&column->type, sizeof(enum DataType), 1, file);

            switch (column->type) {
                case INT32:
                    fread(&column->int32Value, sizeof(int32_t), 1, file);
                    break;
                case FLOAT:
                    fread(&column->floatValue, sizeof(float), 1, file);
                    break;
                case CHAR_PTR:
                    {
						fread(column->charValue, sizeof(MAX_CHAR_VALUE), 1, file);
                        break;  
                    }
                case BOOL:
                    fread(&column->boolValue, sizeof(bool), 1, file);
                    break;
                default:
                    break;
            }
        }
    }

    fclose(file);
}

void loadHeaderFromFile(struct GraphDB* db, const char* fileName) {

    FILE* file = fopen(fileName, "rb");
    
    if (file == NULL) {
        printf("Error opening file (probably it not exists'): %s\n", fileName);
        return;
    }

    // Read the number of columns in the scheme
    int schemeCount;
    fread(&schemeCount, sizeof(int), 1, file);

    // Read the data type of each column in the scheme
    db->scheme = malloc(schemeCount * sizeof(struct Column));
    db->schemeCount = schemeCount;
    int k;
    for (k = 0; k < schemeCount; k++) {
        fread(&db->scheme[k].type, sizeof(enum DataType), 1, file);
        fread(&db->scheme[k].name, (size_t)MAX_NAME_LENGTH, 1, file);
    }
    
    int rowCount;
    fread(&rowCount, sizeof(int), 1, file);
    
    db->rows = malloc(rowCount * sizeof(struct Row));
    db->rowCount = rowCount;
    
    fclose(file);
}









