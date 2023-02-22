#ifndef LLP2_GREMLIN_STRUCTURE_H
#define LLP2_GREMLIN_STRUCTURE_H
#include <inttypes.h>

#define MAX_NAME_SIZE 20
#define MAX_ARRAY_SIZE 20

enum crud_operation {
    CRUD_GET = 0,
    CRUD_REMOVE,
    CRUD_NEW,
    CRUD_UPDATE
};

enum condition_code {
    OP_EQUAL = 0,
    OP_GREATER,
    OP_LESS,
    OP_NOT_GREATER,
    OP_NOT_LESS,
    OP_SUBSTR,
};

enum types {
    STRING_TYPE = 0,
    INTEGER_TYPE,
    REAL_TYPE,
    BOOLEAN_TYPE
};

union field_types{
    char string[MAX_NAME_SIZE];
    int64_t integer;
    int64_t boolean;
    double real;
};

struct field {
    enum types type;
    char name[MAX_NAME_SIZE];
    union field_types value;
};

struct entity {
    struct field fields[MAX_ARRAY_SIZE];
    uint64_t rel[MAX_ARRAY_SIZE];
    uint8_t fields_count;
    uint8_t rel_count;
};

struct condition {
    uint8_t is_id;
    uint64_t id;
    enum types type;
    enum condition_code op;
    char field_name[MAX_NAME_SIZE];
    union field_types field_value;
    struct condition *next;
};

struct filter_list {
    uint8_t negative;
    struct condition *condition;
    struct filter_list *next;
};

struct list_level {
    struct list_level *sib;
    struct filter_list *filters;
};

struct view {
    enum crud_operation op;
    struct list_level *tree;
    struct entity *entity;
};

#endif
