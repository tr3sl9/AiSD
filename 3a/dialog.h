#ifndef DIALOG_H
#define DIALOG_H
#include "table.h"
typedef table_err (*functions)(struct Table*);

void show_menu();
int process_choice(Table* const, size_t choice);

#endif
