#ifndef DIALOG_H
#define DIALOG_H
#include "../lib/libgraph.h"

typedef graph_err (*graph_func)(Graph* const);

typedef struct {
    graph_func func;
    char *msg;
} operation;

void show_menu(void);
int process_choice(Graph* const graph, size_t choice);
size_t get_operation_count(void);
#endif
