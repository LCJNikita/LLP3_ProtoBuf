#include "printer.h"

void print_view(struct view *view) {
    char *op;
    switch (view->op) {
        case CRUD_NEW:
            op = "NEW";
            break;
        case CRUD_UPDATE:
            op = "UPDATE";
            break;
        case CRUD_REMOVE:
            op = "REMOVE";
            break;
        case CRUD_GET:
            op = "GET";
            break;
        default:
            op = "NULL";
            break;
    }
    printf("\n--- OPERATION ---\n OPERATION IS %s\n\n", op);
    printf("--- ENTITY ---\n");
    printf("FIELDS:\n");
    for (size_t i = 0; i < view->entity->fields_count; i++) {
        if (view->entity->fields[i].type == STRING_TYPE) {
            printf("\t%s: %s\n", view->entity->fields[i].name, view->entity->fields[i].value.string);
        } else {
            printf("\t%s: %ld\n", view->entity->fields[i].name, view->entity->fields[i].value.integer);
        }
    }
    printf("RELATIONS:\n\t");
    for (size_t i = 0; i < view->entity->rel_count; i++) {
        printf("%ld\n", view->entity->rel[i]);
    }
    printf("\n--- FILTERS ---\n");
    struct list_level *level = view->tree;
    size_t level_count = 0, filter_count = 0, condition_count = 0;
    struct filter_list *filter;
    struct condition *condition;
    while (level != NULL) {
        filter = level->filters;
        printf("\tLEVEL %zu:\n", level_count);
        filter_count = 0;
        while (filter != NULL) {
            printf("\t\tFILTER %zu:\n", filter_count);
            if (filter->negative) printf("\t\t\tIt is negative\n");
            condition = filter->condition;
            condition_count = 0;
            while (condition != NULL) {
                printf("\t\t\tCONDITION %zu:\n", condition_count);
                if (condition->is_id) {
                    printf("\t\t\t\tCONDITION BY ID %ld\n", condition->id);
                } else {
                    switch (condition->op) {
                        case OP_EQUAL:
                            op = "=";
                            break;
                        case OP_LESS:
                            op = "<";
                            break;
                        case OP_GREATER:
                            op = ">";
                            break;
                        case OP_NOT_LESS:
                            op = ">=";
                            break;
                        case OP_NOT_GREATER:
                            op = "<=";
                            break;
                        case OP_SUBSTR:
                            op = "substring";
                            break;
                        default:
                            op = "NULL";
                            break;
                    }
                    printf("\t\t\t\tOPERATION IS %s:\n", op);
                    if (condition->type == STRING_TYPE) {
                        printf("\t\t\t\t%s: %s\n", condition->field_name, condition->field_value.string);
                    } else {
                        printf("\t\t\t\t%s: %ld\n", condition->field_name, condition->field_value.integer);
                    }
                }
                condition = condition->next;
                condition_count++;
            }
            filter = filter->next;
            filter_count++;
        }
        level = level->sib;
        level_count++;
    }
}
