#ifndef TABLE_H
#define TABLE_H

#include <stdio.h>
#include <stdint.h>

#include "my_struct.h"
#include "info_struct.h"

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

typedef enum BusyType {
    EMPTY = 0,
    BUSY = 1,
    DELETED = 2
} BusyType;

typedef struct KeySpace {
    BusyType busy;
    char* key;
    size_t release;
    Info* info;
} KeySpace;

typedef struct Table {
    KeySpace *ks;
    size_t msize;
    size_t csize;
} Table;

Table *create_table(const size_t);

void free_table(Table * const);
void free_ks(KeySpace * const);

table_err insert_key_to_table(Table * const, const char * const key, const size_t info);
table_err delete_key_from_table(Table * const, const char * const key);

table_err print_table(const Table * const);

KeySpace* search_by_key_with_release_in_table(const Table * const, const char * const key, const size_t release);
KeySpace** search_by_key_in_table(const Table * const, const char * const key, size_t * const count_key);

table_err import_table_from_file(Table * const, const char * const filename);
table_err export_table_to_file(const Table * const, const char * const filename);

#endif
