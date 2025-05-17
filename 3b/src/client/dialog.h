#ifndef DIALOG_H
#define DIALOG_H
#include "../lib/table.h"

typedef table_err (*functions)(struct Table*);

typedef struct {
    functions func;
    char *msg;
} operation;

void show_menu();
int process_choice(Table* const, size_t choice);

#endif
