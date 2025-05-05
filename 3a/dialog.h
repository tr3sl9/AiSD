#ifndef DIALOG_H
#define DIALOG_H
#include "table.h"
typedef table_err (*functions)(struct Table*);

table_err dialog_insert(Table* const);
table_err dialog_delete(Table* const);
table_err dialog_print(Table* const);
table_err dialog_find(Table* const);
table_err dialog_find_release(Table* const);
table_err dialog_import(Table* const);
table_err dialog_clean(Table* const);
table_err dialog_export(Table* const);
table_err dialog_export(Table* const);
table_err dialog_exit(Table* const);
void show_menu();
int process_choice(Table* const, size_t choice);

#endif
