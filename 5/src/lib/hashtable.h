#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "libgraph.h"

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
    Vertex *vertex; 
} KeySpace;

typedef struct Table {
    KeySpace *ks;
    size_t msize;
    size_t csize;
} Table;

Table* create_table(const size_t);

void table_free(Table * const);

KeySpace* search_vertex_in_table(const Table * const, const char * const id);

table_err insert_vertex_table(Table * const, Vertex * const vertex);
table_err delete_vertex_table(Table * const, const char * const str);

#endif
