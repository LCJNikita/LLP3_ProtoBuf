/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.7 */

#ifndef PB_MESSAGE_PB_H_INCLUDED
#define PB_MESSAGE_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _Example {
    int32_t value;
} Example;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define Example_init_default                     {0}
#define Example_init_zero                        {0}

/* Field tags (for use in manual encoding/decoding) */
#define Example_value_tag                        1

/* Struct field encoding specification for nanopb */
#define Example_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, INT32,    value,             1)
#define Example_CALLBACK NULL
#define Example_DEFAULT NULL

extern const pb_msgdesc_t Example_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define Example_fields &Example_msg

/* Maximum encoded size of messages (where known) */
#define Example_size                             11

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
