#ifndef DIALOG_H
#define DIALOG_H
#include "table.h"
#define COUNT_OP 10
typedef void (*functions)(struct Table*);

void dialog_insert(Table*);
void dialog_delete(Table*);
void dialog_print(Table*);
void dialog_find(Table*);
void dialog_find_release(Table*);
void dialog_import(Table*);
void dialog_clean(Table*);
void dialog_export(Table*);
void dialog_export(Table*);
void dialog_init_table(Table*);
void show_menu();
void dialog_exit(Table*);
void process_choice(Table*, size_t choice);

#endif
