#ifndef DIALOG_H
#define DIALOG_H
#include "../lib/bst_lib.h"

typedef tree_err (*functions)(BST*);

typedef struct {
    functions func;
    char *msg;
} operation;

void show_menu();
int process_choice(BST* const, size_t choice);
#endif
