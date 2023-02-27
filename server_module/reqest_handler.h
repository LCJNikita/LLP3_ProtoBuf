#ifndef LCJNIKITA_REQEST_HANDLER_H
#define LCJNIKITA_REQEST_HANDLER_H

#include "graph.h"

void parseAddRequest(View v, const char* filename, char** response);
void parseGetRequest(View v, const char* filename, char** response);
void parseRemoveRequest(View v, const char* filename, char** response);
void parseUpdateRequest(View v, const char* filename, char** response);

#endif
