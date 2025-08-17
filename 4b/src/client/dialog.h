#ifndef DIALOG_H
#define DIALOG_H
#include "../lib/sgt_lib.h"

typedef tree_err (*functions)(SGT*);

typedef struct {
    functions func;
    char *msg;
} operation;

void show_menu();
int process_choice(SGT* const, size_t choice);
#endif
