cmake_minimum_required(VERSION 3.16)
project(LCJNikita C)

set(CMAKE_C_STANDARD 11)

add_subdirectory(gremlin_module)
add_subdirectory(proto_module/nanopb)

include_directories(gremlin_module/include)
include_directories(proto_module/nanopb)
include_directories(proto_module)

#add_executable(LLP3_server server.c)
add_executable(LLP3_client client.c
        proto_module/nanopb/pb_encode.c
        proto_module/nanopb/pb_decode.c
        proto_module/nanopb/pb_common.c
        proto_module/message.pb.c)
#target_link_libraries(LLP3_server gremlin_lib)
target_link_libraries(LLP3_client gremlin_lib)