#ifndef LLP2_GREMLIN_ALLOC_VIEW_H
#define LLP2_GREMLIN_ALLOC_VIEW_H

#include "structure.h"
#include "stdlib.h"

void create_view(struct view **view);

void create_level(struct list_level **level);

void create_filter(struct filter_list **filter);

void create_condition(struct condition **condition);

void append_condition(struct condition *condition, struct list_level *level);

void append_filter(struct list_level *level);

void append_level(struct view *view);

#endif
