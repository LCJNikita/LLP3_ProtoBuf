#include "reqest_handler.h"

void parseAddRequest(View v, FILE* file, char **response) {
    char *inputString = "";
    char *converted = malloc(STR_LEN);
    struct GraphDB db;
    loadHeaderStructFromFile(&db, file);

    if (v.entity.fields_count > db.columnsCount){
        sprintf(converted, "Error while adding!\n%d fields expected // %d received!\n", db.columnsCount, v.entity.fields_count);
        *response = concat(*response, converted);
        return;
    }else if (v.entity.fields_count < db.columnsCount){
        *response = concat(*response, "Missing fields detected. They will be filled with default values.\n");
    }

    for (int col_iter = 0; col_iter < db.columnsCount; ++col_iter) {
        for (int i = 0; i < v.entity.fields_count; i++) {
            if (v.entity.fields[i].type != -1 && strcmp(v.entity.fields[i].name, db.scheme[col_iter].name) == 0) {
                toString(v.entity.fields[i], &converted);
                inputString = concat(inputString, converted);
                v.entity.fields[i].type = -1;
                if (col_iter < db.columnsCount - 1)
                    inputString = concat(inputString, ",");
                break;
            }else if (i == v.entity.fields_count - 1){
                sprintf(converted, "Warning: missing field '%s' if filled with default value!\n", db.scheme[col_iter].name);
                *response = concat(*response, converted);
                setDefault(db.scheme[col_iter].type, &converted);
                inputString = concat(inputString, converted);
                if (col_iter < db.columnsCount - 1)
                    inputString = concat(inputString, ",");
            }

        }
    }

    for (int i = 0; i < v.entity.fields_count; i++) {
        if (v.entity.fields[i].type != -1){
            sprintf(converted, "Warning: database doesn't have field '%s'! It's value will be ignored.\n", v.entity.fields[i].name);
            *response = concat(*response, converted);
        }
    }


    struct Node node;
    if (parseAndSetNode(&db, inputString, &node, response)) {
        addNodeToFile(filename, &node);
        for (int i = 0; i < v.entity.rel_count; i++) {
            setNewRelation(filename, db.nodesCount, (int) v.entity.rel[i]);
        }

        int *res;
        if (v.tree_count == 1 && v.tree[0].filters_count == 0){ // чтобы выборка дала пустое множество вместо всей бд
            v.tree_count = 0;
        }
        size_t res_cnt = findNodesByFilters(filename, v, &res);
        for (int i = 0; i < res_cnt; i++) {
            setNewRelation(filename, db.nodesCount, (int) res[i]);
        }

        findNodeByIndex(filename, db.nodesCount, &node);
        printNode(&db, &node);
        *response = concat(*response, "Successfully added!\n");
    } else {
        *response = concat(*response, "Error while adding!\n");
    }
}

void parseGetRequest(View v, const char *filename, char **response) {
    int *res;
    struct Node node;
    char *tmp[STR_LEN];
    struct GraphDB db;
    loadHeaderStructFromFile(&db, filename);

    size_t res_cnt = findNodesByFilters(filename, v, &res);
    if (res_cnt > 0) {

        sprintf((char *) tmp, "__________________\n"
                              "Found %zu node(s):\n", res_cnt);
        *response = concat(*response, (const char *) tmp);
    } else
        *response = concat(*response, "__________________\n"
                                      "No elements found.\n");

    for (int i = 0; i < res_cnt; i++) {
        findNodeByIndex(filename, res[i], &node);
        sprintf((char *) tmp, "\n---- NODE %d ----\n", res[i]);
        *response = concat(*response, (const char *) tmp);
        printNodeToVar(db, node, response);
    }
};

void parseRemoveRequest(View v, const char *filename, char **response) {
    int *res;
    size_t res_cnt = findNodesByFilters(filename, v, &res);
    for (int i = 0; i < res_cnt; i++) {
        deleteNodeByIndex(filename, res[i]);
    }
    if (res_cnt > 0) {
        char *tmp[STR_LEN];
        sprintf((char *) tmp, "Successfully removed %zu node(s)!\n", res_cnt);
        *response = concat(*response, (const char *) tmp);
    } else
        *response = concat(*response, "No elements found to remove.\n");
}

void parseUpdateRequest(View v, const char *filename, char **response) {
    int *res;
    size_t res_cnt = findNodesByFilters(filename, v, &res);
    for (int i = 0; i < res_cnt; i++) {
        for (int j = 0; j < v.entity.fields_count; j++) {
            char *converted = malloc(sizeof(char) * STR_LEN);
            toString(v.entity.fields[j], &converted);
            updateNodeByIndex(filename, v.entity.fields[j].name, converted, res[i]);
            free(converted);
        }
        if (v.entity.rel_count != 0) {
            clearAllRelationsOfNode(filename, res[i]);
            for (int k = 0; k < v.entity.rel_count; k++) {
                setNewRelation(filename, res[i], (int) v.entity.rel[k]);
            }
        }
    }
    if (res_cnt > 0) {
        char *tmp[STR_LEN];
        sprintf((char *) tmp, "Successfully updated %zu node(s)!\n", res_cnt);
        *response = concat(*response, (const char *) tmp);
    } else
        *response = concat(*response, "No elements found to update.\n");

}