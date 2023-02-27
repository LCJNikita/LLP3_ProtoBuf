#include <pb_encode.h>
#include <pb_decode.h>
#include "proto_module/message.pb.h"
#include "proto_module/common.h"
#include "server_module/graph.h"
#include "structure.h"
#include "string_tools.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 3939

int main(int argc, char **argv) {
    setbuf(stdout, 0);

    int listenfd, connfd;
    struct sockaddr_in servaddr;
    int reuse = 1;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    servaddr.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
        perror("bind");
        return 1;
    }

    if (listen(listenfd, 5) != 0) {
        perror("listen");
        return 1;
    }

    printf("server: waiting for connection…\n");
    connfd = accept(listenfd, NULL, NULL);

    if (connfd < 0) {
        perror("accept");
        return 1;
    }

    printf("Got connection.\n");

    pb_istream_t input = pb_istream_from_socket(connfd);
    pb_ostream_t output = pb_ostream_from_socket(connfd);

    clearFileData("data.bin");
    struct GraphDB db;
    db.nodesCount = 0;
    db.columnsCount = 0;
    db.nodes = NULL;
    db.scheme = NULL;

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

    setScheme(&db, scheme, columnsCount);

    saveHeaderStructToFile(&db, "data.bin");

    createNode("data.bin", "10000,228.56,a,false");
    createNode("data.bin", "10000,228.56,b,false");
    createNode("data.bin", "10000,228.56,c,false");

    setNewRelation("data.bin", 0, 1);
    setNewRelation("data.bin", 0, 2);
    setNewRelation("data.bin", 1, 2);
//
//	deleteNodeByIndex("data.bin", 1);

    char *response;
    while (1) {
        response = "";
        View v = {};
        if (!pb_decode_delimited(&input, View_fields, &v)) {
            printf("Decode failed: %s\n", PB_GET_ERROR(&input));
            return 2;
        }


        struct GraphDB db;
        char *filename = "data.bin";
        loadHeaderStructFromFile(&db, filename);

        switch (v.op) {
            case CRUD_GET: {
                int *res;
                struct Node node;
                size_t res_cnt = findNodesByFilters(filename, v, &res);
                if (res_cnt > 0) {
                    char* tmp[STR_LEN];
                    sprintf(tmp, "__________________\n"
                                 "Found %zu node(s):\n", res_cnt);
                    response = concat(response, tmp);
                } else
                    response = concat(response, "__________________\n"
                                                "No elements found.\n");

                for (int i = 0; i < res_cnt; i++) {
                    findNodeByIndex(filename, res[i], &node);
                    printNodeToVar(db, node, &response);
                }
                break;
            }
            case CRUD_NEW: {
                char *inputString = "";
                char *converted = malloc(sizeof(char) * STR_LEN);

                for (int i = 0; i < v.entity.fields_count; i++) {
                    toString(v.entity.fields[i], &converted);
                    inputString = concat(inputString, converted);

                    if (i < v.entity.fields_count - 1)
                        inputString = concat(inputString, ",");

                }

                struct Node node;
                if (parseAndSetNode(&db, inputString, &node)) {
                    addNodeToFile(filename, &node);
                    for (int i = 0; i < v.entity.rel_count; i++) {
                        setNewRelation(filename, db.nodesCount, (int) v.entity.rel[i]);
                    }

                    int *res;
                    size_t res_cnt = findNodesByFilters(filename, v, &res);
                    for (int i = 0; i < res_cnt; i++) {
                        setNewRelation(filename, db.nodesCount, (int) res[i]);
                    }

                    findNodeByIndex(filename, db.nodesCount, &node);
                    printNode(&db, &node);
                    response = concat(response, "Successfully added!\n");
                } else {
                    response = concat(response, "Error while adding!\n");
                }
                break;
            }
            case CRUD_REMOVE: {
                int *res;
                size_t res_cnt = findNodesByFilters(filename, v, &res);
                for (int i = 0; i < res_cnt; i++) {
                    deleteNodeByIndex(filename, res[i]);
                }
                if (res_cnt > 0) {
                    char* tmp[STR_LEN];
                    sprintf(tmp, "Successfully removed %zu node(s)!\n", res_cnt);
                    response = concat(response, tmp);
                } else
                    response = concat(response, "No elements found to remove.\n");
                break;
            }
            case CRUD_UPDATE: {
                int *res;
                size_t res_cnt = findNodesByFilters(filename, v, &res);
                for (int i = 0; i < res_cnt; i++) {
                    for (int j = 0; j < v.entity.fields_count; j++) {
                        char* converted = malloc(sizeof(char) * STR_LEN);
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
                    char* tmp[STR_LEN];
                    sprintf(tmp, "Successfully updated %zu node(s)!\n", res_cnt);
                    response = concat(response, tmp);
                } else
                    response = concat(response, "No elements found to update.\n");

                break;
            }
        }


        Response r = {};
        while (strlen(response) > 63) {
            strncpy(r.answer, response, 63);
            r.answer[63] = 0;
            r.is_last = 0;
            if (!pb_encode_delimited(&output, Response_fields, &r)) {
                fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&output));
            }
            response += 63;
        }
        strcpy(r.answer, response);
        r.is_last = 1;
        if (!pb_encode_delimited(&output, Response_fields, &r)) {
            fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&output));
        }


    }

}

