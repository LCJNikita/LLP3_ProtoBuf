#include <stdio.h>
#include "gremlin_module/include/view_tools/parser_pieces.h"
#include "gremlin_module/include/stools/printer.h"
#include <pb_encode.h>
#include "proto_module/message.pb.h"

int main() {
//    struct view *view = parse_request();
//    if (view != NULL) {
//        print_view(view);
//    }

    Example mymessage = {42};
    uint8_t buffer[10];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&stream, Example_fields, &mymessage);

    return 0;
}
