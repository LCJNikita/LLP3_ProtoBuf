#include "alloc_view.h"

void create_view(struct view **view){
    *view = malloc(sizeof(struct view));
    (*view)->op = CRUD_GET;
    (*view)->entity = NULL;
    (*view)->tree = NULL;
}

void create_level(struct list_level **level){
    *level = malloc(sizeof(struct list_level));
    (*level)->filters = NULL;
    (*level)->sib = NULL;
}

void create_filter(struct filter_list **filter){
    *filter = malloc(sizeof(struct filter_list));
    (*filter)->negative = 0;
    (*filter)->condition = NULL;
    (*filter)->next = NULL;
}

void create_condition(struct condition **condition){
    *condition = malloc(sizeof(struct condition));
    (*condition)->is_id = 0;
    (*condition)->id = 0;
    (*condition)->next = NULL;
    (*condition)->op = OP_EQUAL;
}

void append_condition(struct condition *condition, struct list_level *level) {
    if (level->filters == NULL) {
        struct filter_list *filter;
        create_filter(&filter);
        level->filters = filter;
    }
    condition->next = level->filters->condition;
    level->filters->condition = condition;
}

void append_filter(struct list_level *level) {
    struct filter_list *filter;
    create_filter(&filter);
    filter->next = level->filters;
    level->filters = filter;
}

void append_level(struct view *view) {
    struct list_level *level;
    create_level(&level);
    level->sib = view->tree;
    view->tree = level;
}