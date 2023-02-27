#ifndef LCJNIKITA_STRING_TOOLS_H
#define LCJNIKITA_STRING_TOOLS_H

#include <malloc.h>
#include <string.h>
#include "../proto_module/message.pb.h"

#define STR_LEN 64
void toString(Field f, char *res[STR_LEN]);
char *concat(const char *s1, const char *s2);

#endif
