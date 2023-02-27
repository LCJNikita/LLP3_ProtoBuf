#include <stdio.h>
#include "gremlin_module/include/view_tools/parser_pieces.h"
#include "gremlin_module/include/stools/printer.h"
#include <pb_encode.h>
#include <pb_decode.h>
#include "proto_module/message.pb.h"
#include "proto_module/common.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

void parse_view(struct view view, View *view_new);

int main(int argc, char *argv[]) {
    setbuf(stdout, 0);

    if (argc != 3) {
        printf("Expected 2 parameters: <host address> <port>\n Found %d.\n", argc - 1);
        return 1;
    }

    struct sockaddr_in servaddr;
    int sockfd;
    size_t port;
    port = strtol(argv[2], NULL, 10);

    if (port == 0) {
        printf("Invalid port.");
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
        perror("connect");
        return 1;
    }

    printf("client: connecting...\n");

    while (1) {
        struct view *view = parse_request();
        if (view != NULL) {
//            print_view(view);

            View view_new = View_init_zero;
            parse_view(*view, &view_new);

            pb_ostream_t output = pb_ostream_from_socket(sockfd);
            if (!pb_encode_delimited(&output, View_fields, &view_new)) {
                fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&output));
                return 2;
            }

            Response r = {};
            pb_istream_t input = pb_istream_from_socket(sockfd);
            if (!pb_decode_delimited(&input, Response_fields, &r)) {
                printf("Decode failed: %s\n", PB_GET_ERROR(&input));
                return 2;
            }
            printf("%s", r.answer);

            while(!r.is_last){
                if (!pb_decode_delimited(&input, Response_fields, &r))
                {
                    printf("Decode failed: %s\n", PB_GET_ERROR(&input));
                    return 2;
                }
                printf("%s", r.answer);
            }
        }

    }
    close(sockfd);
    return 0;
}

void parse_view(struct view view, View *view_new) {
    view_new->op = view.op;
    size_t filter_count, condition_count, level_count = 0;
    while (view.tree) {
        List_level l = List_level_init_zero;

        filter_count = 0;
        while (view.tree->filters) {
            Filter f = Filter_init_zero;
            f.negative = view.tree->filters->negative;

            condition_count = 0;
            while (view.tree->filters->condition) {
                Condition c = Condition_init_zero;
                c.is_id = view.tree->filters->condition->is_id;
                c.id = view.tree->filters->condition->id;
                c.type = view.tree->filters->condition->type;
                c.op = view.tree->filters->condition->op;
                strcpy(c.field_name, view.tree->filters->condition->field_name);
                Field_value fv = Field_value_init_zero;
                switch (c.type) {
                    case 0:
                        strcpy(fv.str_val, view.tree->filters->condition->field_value.string);
                        fv.has_str_val = 1;
                        break;
                    case 1:
                        fv.int_val = view.tree->filters->condition->field_value.integer;
                        fv.has_int_val = 1;
                        break;
                    case 2:
                        fv.real_val = view.tree->filters->condition->field_value.real;
                        fv.has_real_val = 1;
                        break;
                    case 3:
                        fv.bool_val = view.tree->filters->condition->field_value.boolean;
                        fv.has_bool_val = 1;
                        break;
                }
                c.val = fv;
                f.conditions[condition_count++] = c;
                view.tree->filters->condition = view.tree->filters->condition->next;
            }
            f.conditions_count = condition_count;
            l.filters[filter_count++] = f;

            view.tree->filters = view.tree->filters->next;
        }
        l.filters_count = filter_count;
        view_new->tree[level_count++] = l;

        view.tree = view.tree->sib;
    }
    view_new->tree_count = level_count;
    Entity e = Entity_init_zero;


    if (view_new->op == 2 || view_new->op == 3) {
        view_new->has_entity = 1;
        e.fields_count = view.entity->fields_count;
        e.rel_count = view.entity->rel_count;

        for (int rc = 0; rc < e.rel_count; rc++) {
            e.rel[rc] = view.entity->rel[rc];
        }

        for (int fc = 0; fc < e.fields_count; fc++) {
            Field field = Field_init_zero;
            field.type = view.entity->fields[fc].type;
            Field_value fv = Field_value_init_zero;
            switch (field.type) {
                case 0:
                    strcpy(fv.str_val, view.entity->fields[fc].value.string);
                    fv.has_str_val = 1;
                    break;
                case 1:
                    fv.int_val = view.entity->fields[fc].value.integer;
                    fv.has_int_val = 1;
                    break;
                case 2:
                    fv.real_val = view.entity->fields[fc].value.real;
                    fv.has_real_val = 1;
                    break;
                case 3:
                    fv.bool_val = view.entity->fields[fc].value.boolean;
                    fv.has_bool_val = 1;
                    break;
            }
            field.val = fv;
            strcpy(field.name, view.entity->fields[fc].name);

            e.fields[fc] = field;
        }
    }
    view_new->entity = e;
}