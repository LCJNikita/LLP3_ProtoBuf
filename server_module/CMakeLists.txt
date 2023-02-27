cmake_minimum_required(VERSION 3.16)
project(LLP2_Gremlin C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_CURRENT_SOURCE_DIR .)
set(SOURCES graph.c string_tools.c reqest_handler.c reqest_handler.h)
set(HEADERS graph.h string_tools.h reqest_handler.c reqest_handler.h)

add_library(graph_lib SHARED ${SOURCES} ${HEADERS})
target_include_directories(graph_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})