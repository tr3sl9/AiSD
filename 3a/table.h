#ifndef TABLE_H
#define TABLE_H
#include <stdio.h>
#include "table.h"

typedef enum table_err {
	TABLE_OK = 0,
	TABLE_EMPTY = 1,
	TABLE_FULL = 2,
	TABLE_MEM = 3,
	TABLE_NULL = 4, 
	TABLE_VAL = 5,
	TABLE_SIZE = 6,
	TABLE_MAGIC_WORD = 7,
	FILE_ERR = 8,
	TABLE_EXIT = 9,
	TABLE_EOF = -1
} table_err;

typedef struct KeySpace {
	char* key;
	size_t release;
	char* info;
} KeySpace;

typedef struct Table {
	KeySpace *ks;
	size_t msize;
	size_t csize;
} Table;

void init_key_space(KeySpace *ks);
int table_initialized(const Table*);
Table *create_table(const size_t);
void free_table(Table*);
table_err insert_key_to_table(Table*, const char * key, const char * info);
table_err delete_key_from_table(Table*, const char*);
table_err print_table(const Table*);
table_err import_table_from_file(Table*, const char*);
table_err export_table_to_file(const Table*, const char*);
Table* search_by_key_in_table(const Table*, const char*);
Table* search_by_key_with_release_in_table(const Table*, const char*, const size_t);
table_err clean_table(Table*);

#endif
