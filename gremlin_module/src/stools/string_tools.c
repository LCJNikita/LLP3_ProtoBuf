#include "string_tools.h"

static uint64_t get_hash(const char *string){
    uint64_t value = INITHASH;
    while(*string) value = (value * AHASH) ^ (*(string++) * BHASH);
    return value % CHASH;
}

uint8_t parse_command(char *command) {
    uint64_t hash = get_hash(command);
    for (size_t iter = 0; iter < COMMAND_COUNT; iter++){
        if (commands_hash[iter] == hash) return iter;
    }
    return WRONG_COMMAND;
}

void init_string_tools() {
    commands_hash[0] = get_hash("as");
    commands_hash[1] = get_hash("has");
    commands_hash[2] = get_hash("or");
    commands_hash[3] = get_hash("not");
    commands_hash[4] = get_hash("out");
    commands_hash[5] = get_hash("get");
    commands_hash[6] = get_hash("remove");
    commands_hash[7] = get_hash("new");
    commands_hash[8] = get_hash("update");
    commands_hash[9] = get_hash("value");
    commands_hash[10] = get_hash("rel");
}

uint8_t is_char(char c) {
    return c > 64 && c < 91 || c > 96 && c < 123;
}

uint8_t is_int(char c) {
    return c > 47 && c < 58;
}
