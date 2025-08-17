#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/sgt_lib.h"
#include "dialog.h"
#include "testing.h"

#define OPERATION_PROMPT "Choice operation: "
#define ERR_PROMPT "Error: Cannot create tree\n"
#define USAGE_PROMPT "Usage: %s <alpha>\nWhere alpha is a number between 0.5 and 1.0 (exclusive)\n"
#define ALPHA_RANGE_ERR "Error: Alpha must be between 0.5 and 1.0 (exclusive)\n"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf(USAGE_PROMPT, argv[0]);
        return 1;
    }

    double alpha = atof(argv[1]);
    if (alpha < 0.5 || alpha >= 1.0) {
        printf(ALPHA_RANGE_ERR);
        return 1;
    }

    SGT *tree = create_tree(alpha);
    if (!tree) {
        printf(ERR_PROMPT);
        return 1;
    }

    int end_program = 0;
    while (!end_program) {
        show_menu();
        size_t choice;
        if (read_number(&choice, 0, 11, OPERATION_PROMPT) == TREE_EOF) {
            free_tree(tree);
            return 1;
        }
        end_program = process_choice(tree, choice);
    }

    free_tree(tree);
    return 0;
}
