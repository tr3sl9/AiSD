#include <stdio.h>
#include <stdlib.h>
#include "../lib/bst_lib.h"
#include "dialog.h"
#include "testing.h"

#define PROMPT_FOR_OPERATION "Choice operation: "

int main(void) {
    BST *tree = create_tree();
    if (!tree) {
        printf("Error: Cannot create tree\n");
        return 1;
    }

    int end_program = 0;
    while (!end_program) {
        show_menu();
        size_t choice;
        if (read_number(&choice, 0, 11, PROMPT_FOR_OPERATION) == TREE_EOF) {
            free_tree(tree);
            return 1;
        }
        end_program = process_choice(tree, choice);
    }

    free_tree(tree);
    return 0;
}
