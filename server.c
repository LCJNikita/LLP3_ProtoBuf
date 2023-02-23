#include <pb_encode.h>
#include <pb_decode.h>
#include "proto_module/message.pb.h"
#include "proto_module/common.h"
#include "server_module/graph.h"
#include "structure.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 3939
#define STR_LEN 64

char *concat(const char *s1, const char *s2);

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

    char *response = "";
    while (1) {
        View v = {};
        if (!pb_decode_delimited(&input, View_fields, &v)) {
            printf("Decode failed: %s\n", PB_GET_ERROR(&input));
            return 2;
        }


        struct GraphDB db;
        char *filename = "data.bin";
        loadHeaderStructFromFile(&db, filename);\

        switch (v.op) {
            case CRUD_GET: {
                int *res;
                size_t res_cnt = findRowsByFilters(filename, v, &res);
                for (int i = 0; i < res_cnt; i++) {
                    printf("%d ", res[i]);
                }
                break;
            }
            case CRUD_NEW: {
                char *inputString = "";
                char converted[STR_LEN];

                for (int i = 0; i < v.entity.fields_count; i++) {
                    switch (v.entity.fields[i].type) {
                        case 0:
                            sprintf(converted, "%s", v.entity.fields[i].val.str_val);
                            break;
                        case 1:
                            sprintf(converted, "%ld", v.entity.fields[i].val.int_val);
                            break;
                        case 2:
                            sprintf(converted, "%f", v.entity.fields[i].val.real_val);
                            break;
                        case 3:
                            if (v.entity.fields[i].val.bool_val)
                                sprintf(converted, "true");
                            else
                                sprintf(converted, "false");
                            break;

                    }
                    inputString = concat(inputString, converted);

                    if (i < v.entity.fields_count - 1)
                        inputString = concat(inputString, ",");

                }

                struct Row row;
                if (parseAndSetRow(&db, inputString, &row)) {
                    addRowToFile("data.bin", &row);
                    printRow(&db, &row);
                    response = concat(response, "Successfully added!\n");
                } else {
                    response = concat(response, "Error while adding!\n");
                }
            }
                break;
            case CRUD_REMOVE:
                break;
            case CRUD_UPDATE:
                break;
        }


        Response r = {};
        while (strlen(response) > 64) {
            strncpy(r.answer, response, 64);
            r.answer[63] = 0;
            r.is_last = 0;
            if (!pb_encode_delimited(&output, Response_fields, &r)) {
                fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&output));
            }
            response += 64;
        }
        strcpy(r.answer, response);
        r.is_last = 1;
        if (!pb_encode_delimited(&output, Response_fields, &r)) {
            fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&output));
        }


    }
}

char *concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}