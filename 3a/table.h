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

typedef char* KeyType;
typedef char* InfoType;
typedef size_t RelType;
typedef size_t IndexType;

typedef struct KeySpace {
	KeyType key;
	RelType release;
	InfoType info;
} KeySpace;

typedef struct Table {
	KeySpace *ks;
	IndexType msize;
	IndexType csize;
} Table;

void init_key_space(KeySpace *ks);
int table_initialized(Table*);
Table *create_table(const IndexType);
void free_table(Table*);
table_err insert_key_to_table(Table*, const KeyType key, const InfoType info);
table_err delete_key_from_table(Table*, const KeyType);
table_err print_table(const Table*);
table_err import_table_from_file(Table*, const char*);
table_err export_table_to_file(const Table*, const char*);
Table* search_by_key_in_table(Table*, const KeyType);
Table* search_by_key_with_release_in_table(Table*, const KeyType, const RelType);
table_err clean_table(Table*);

#endif
