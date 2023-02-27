#include <pb_encode.h>
#include <pb_decode.h>
#include "proto_module/message.pb.h"
#include "proto_module/common.h"
#include "server_module/graph.h"
#include "server_module/reqest_handler.h"
#include "structure.h"
#include "string_tools.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 3939

int main(int argc, char **argv) {
    setbuf(stdout, 0);

    if (argc != 2) {
        printf("Expected 1 parameter: <filename>. Found %d.\n", argc - 1);
        return 1;
    }

    FILE* file = tryOpenFile(argv[1]);

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


    char *response;

    while (1) {
        response = "";
        View v = {};
        if (!pb_decode_delimited(&input, View_fields, &v)) {
            printf("Decode failed: %s\n", PB_GET_ERROR(&input));
            return 2;
        }

        switch (v.op) {
            case CRUD_GET:
                parseGetRequest(v, file, &response);
                break;
            case CRUD_NEW:
                parseAddRequest(v, file, &response);
                break;
            case CRUD_REMOVE:
                parseRemoveRequest(v, file, &response);
                break;
            case CRUD_UPDATE: {
                parseUpdateRequest(v, file, &response);
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

