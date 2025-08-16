#include <stdio.h>
#include <stdlib.h>

#include "../lib/libgraph.h"
#include "dialog.h"
#include "readnumber.h"

#define OPERATION_PROMPT "Choice operation: "
#define ERR_PROMPT "Error: Cannot create graph\n"

int main(void) {
    Graph *graph = graph_create();
    if (!graph) {
        printf(ERR_PROMPT);
        return 1;
    }

    int end_program = 0;
    while (!end_program) {
        show_menu();
        int choice = 0;
        graph_err rn = read_number(&choice, 1, (int)get_operation_count(), OPERATION_PROMPT);
        if (rn != GRAPH_OK) {
            graph_free(graph);
            return rn == GRAPH_EOF ? 0 : 1;
        }
        end_program = process_choice(graph, (size_t)choice);
    }

    graph_free(graph);
    return 0;
}
