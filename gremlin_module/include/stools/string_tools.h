#ifndef LLP2_GREMLIN_STRING_TOOLS_H
#define LLP2_GREMLIN_STRING_TOOLS_H
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define AHASH 54059
#define BHASH 76963
#define CHASH 86969
#define INITHASH 37

#define COMMAND_COUNT 11

uint64_t commands_hash[COMMAND_COUNT];
enum commands_desc{
    AS_COMMAND = 0,
    HAS_COMMAND,
    OR_COMMAND,
    NOT_COMMAND,
    OUT_COMMAND,
    GET_COMMAND,
    REMOVE_COMMAND,
    NEW_COMMAND,
    UPDATE_COMMAND,
    VALUE_COMMAND,
    REL_COMMAND,
    WRONG_COMMAND
};

void init_string_tools();
uint8_t parse_command(char *command);
uint8_t is_int(char c);
uint8_t is_char(char c);

#endif
