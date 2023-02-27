#include "graph.h"

char *concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void toString(Field f, char *res[STR_LEN]) {
    switch (f.type) {
        case 0:
            sprintf(*res, "%s", f.val.str_val);
            break;
        case 1:
            sprintf(*res, "%ld", f.val.int_val);
            break;
        case 2:
            sprintf(*res, "%f", f.val.real_val);
            break;
        case 3:
            if (f.val.bool_val)
                sprintf(*res, "true");
            else
                sprintf(*res, "false");
            break;

    }
}

void setDefault(uint8_t type, char *res[STR_LEN]) {
    switch (type) {
        case CHAR_PTR:
            sprintf(*res, "default");
            break;
        case INT32:
            sprintf(*res, "0");
            break;
        case FLOAT:
            sprintf(*res, "0.0");
            break;
        case BOOL:
            sprintf(*res, "false");
            break;
        default:
            sprintf(*res, "0");

    }
}