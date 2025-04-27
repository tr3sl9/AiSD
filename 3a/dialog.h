#ifndef DIALOG_H
#define DIALOG_H
#include "table.h"
#define COUNT_OP 9
typedef table_err (*functions)(struct Table*);

table_err dialog_insert(Table*);
table_err dialog_delete(Table*);
table_err dialog_print(Table*);
table_err dialog_find(Table*);
table_err dialog_find_release(Table*);
table_err dialog_import(Table*);
table_err dialog_clean(Table*);
table_err dialog_export(Table*);
table_err dialog_export(Table*);
table_err dialog_exit(Table*);
void show_menu();
int process_choice(Table*, size_t choice);

#endif
