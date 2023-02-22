#include "structure.h"
#include "string_tools.h"
#include "alloc_view.h"

#define REQUEST_SIZE 1024

static double parse_real(uint64_t *pointer, char *string) {
    double result = 0;
    int8_t flag = 0;
    double exp = 0.1;
    while (is_int(string[*pointer]) || string[*pointer] == '.') {
        if (flag && string[*pointer] == '.') return result;
        if (string[*pointer] == '.') {
            flag++;
            (*pointer)++;
            continue;
        }
        if (flag) {
            result = result + exp * (string[*pointer] - '0');
            exp /= 10;
        } else {
            result = result * 10 + string[*pointer] - '0';
        }
        (*pointer)++;
    }
    return result;
}

static int8_t parse_bool(uint64_t *pointer, const char *str) {
    if (str[*pointer] == 't' && str[*pointer + 1] == 'r' && str[*pointer + 2] == 'u' && str[*pointer + 3] == 'e') {
        (*pointer) += 4;
        return 1;
    } else if (str[*pointer] == 'f' && str[*pointer + 1] == 'a' && str[*pointer + 2] == 'l' &&
               str[*pointer + 3] == 's' && str[*pointer + 4] == 'e') {
        (*pointer) += 5;
        return 0;
    }
    return 0;
}

static int64_t parse_int(uint64_t *pointer, char *string) {
    int64_t result = 0;
    while (is_int(string[*pointer])) {
        result = result * 10 + (string[*pointer] - '0');
        (*pointer)++;
    }
    return result;
}

static char *parse_string(uint64_t *pointer, char *string) {
    char *str = malloc(sizeof(char) * MAX_NAME_SIZE);
    size_t len = 0;
    while (is_char(string[*pointer]) && len < MAX_NAME_SIZE - 1) {
        str[len] = string[*pointer];
        (*pointer)++;
        len++;
    }
    str[len] = 0;
    return str;
}

static enum condition_code parse_condition(uint64_t *pointer, char *string) {
    switch (string[(*pointer)++]) {
        case '=':
            return OP_EQUAL;
        case '>':
            if (string[*pointer] == '=') {
                (*pointer)++;
                return OP_NOT_LESS;
            } else return OP_GREATER;
        case '<':
            if (string[*pointer] == '=') {
                (*pointer)++;
                return OP_NOT_GREATER;
            } else return OP_LESS;
        case 's':
            if (string[*pointer] == 'u' && string[*pointer + 1] == 'b') {
                (*pointer)++;
                (*pointer)++;
                return OP_SUBSTR;
            } else return OP_EQUAL;
        default:
            return OP_EQUAL;
    }
}

uint8_t parse_operator(uint64_t *pointer, struct view *view, char *string) {
    struct list_level *level;
    if (view->tree == NULL) {
        create_level(&level);
        view->tree = level;
    }
    level = view->tree;
    char str[MAX_NAME_SIZE];
    if (string[*pointer] != '.') return WRONG_COMMAND;
    (*pointer)++;
    size_t len = 0;
    while (is_char(string[*pointer]) && len < MAX_NAME_SIZE - 1) {
        str[len] = string[*pointer];
        (*pointer)++;
        len++;
    }
    if (len == MAX_NAME_SIZE - 1 && is_char(is_char(string[*pointer])) || string[*pointer] != '(') return WRONG_COMMAND;
    str[len] = 0;
    (*pointer)++;
    struct condition *condition;
    create_condition(&condition);
    struct filter_list *filter;
    create_filter(&filter);
    enum commands_desc command = parse_command(str);
    int64_t id;
    char *name, *value;
    enum condition_code code;
    switch (command) {
        case AS_COMMAND:
            id = parse_int(pointer, string);
            condition->is_id = 1;
            condition->id = id;
            append_condition(condition, level);
            break;
        case HAS_COMMAND:
            name = parse_string(pointer, string);
            strcpy(condition->field_name, name);
            if (string[*pointer] != ',') return WRONG_COMMAND;
            (*pointer)++;
            if (string[*pointer] == '"') {
                (*pointer)++;
                value = parse_string(pointer, string);
                strcpy(condition->field_value.string, value);
                condition->type = STRING_TYPE;
                if (string[*pointer] == '"') (*pointer)++;
                else return WRONG_COMMAND;
            } else {
                id = parse_int(pointer, string);
                condition->type = INTEGER_TYPE;
                condition->field_value.integer = id;
            }
            if (string[*pointer] != ',') return WRONG_COMMAND;
            (*pointer)++;
            code = parse_condition(pointer, string);
            condition->op = code;
            append_condition(condition, level);
            break;
        case VALUE_COMMAND:
            name = parse_string(pointer, string);
            strcpy(view->entity->fields[view->entity->fields_count].name, name);
            if (string[*pointer] != ',') return WRONG_COMMAND;
            (*pointer)++;
            if (string[*pointer] == '"') {
                (*pointer)++;
                value = parse_string(pointer, string);
                strcpy(view->entity->fields[view->entity->fields_count].value.string, value);
                view->entity->fields[view->entity->fields_count].type = STRING_TYPE;
                if (string[*pointer] == '"') (*pointer)++;
                else return WRONG_COMMAND;
            } else if (string[*pointer] == 't' || string[*pointer] == 'f') {
                int8_t b = parse_bool(pointer, string);
                view->entity->fields[view->entity->fields_count].value.boolean = b;
                view->entity->fields[view->entity->fields_count].type = BOOLEAN_TYPE;
            } else {
                double real = parse_real(pointer, string);
                if ((double) ((int64_t) real) < real) {
                    view->entity->fields[view->entity->fields_count].value.real = real;
                    view->entity->fields[view->entity->fields_count].type = REAL_TYPE;
                } else {
                    view->entity->fields[view->entity->fields_count].value.integer = real;
                    view->entity->fields[view->entity->fields_count].type = INTEGER_TYPE;
                }
            }
            view->entity->fields_count++;
            break;
        case REL_COMMAND:
            id = parse_int(pointer, string);
            view->entity->rel[view->entity->rel_count] = id;
            view->entity->rel_count++;
            break;
        case OR_COMMAND:
            append_filter(level);
            break;
        case NOT_COMMAND:
            level->filters->negative = 1;
            break;
        case OUT_COMMAND:
            append_level(view);
            break;
        case GET_COMMAND:

            view->op = CRUD_GET;
            break;
        case REMOVE_COMMAND:
            view->op = CRUD_REMOVE;
            break;
        case NEW_COMMAND:
            view->op = CRUD_NEW;
            break;
        case UPDATE_COMMAND:
            view->op = CRUD_UPDATE;
            break;
        default:
            break;
    }
    if (string[*pointer] != ')') return WRONG_COMMAND;
    (*pointer)++;
    return command;
}

struct view *parse_request() {
    init_string_tools();
    struct view *view;
    uint64_t pointer = 0, code;
    uint8_t flag = 0;
    char string[REQUEST_SIZE];
    create_view(&view);
    view->entity = malloc(sizeof(struct entity));
    scanf("%s", string);
    if (string[0] != 'g' || string[1] != '.' || string[2] != 'V') {
        printf("syntax error!\n");
        return NULL;
    }
    pointer = 3;
    while (string[pointer]) {
        code = parse_operator(&pointer, view, string);
        if ((code == VALUE_COMMAND || code == REL_COMMAND) && !flag || code == WRONG_COMMAND) {
            printf("syntax error!\n");
            return NULL;
        }
        if (code == GET_COMMAND || code == REMOVE_COMMAND || code == UPDATE_COMMAND || code == NEW_COMMAND) {
            if (flag) {
                printf("syntax error!\n");
                return NULL;
            } else flag++;
        }
    }
    if (!flag) {
        printf("syntax error!\n");
        return NULL;
    }
    return view;
}