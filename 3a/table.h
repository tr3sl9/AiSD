#ifndef TABLE_H
#define TABLE_H

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
	TABLE_EOF = -1
} table_err;

typedef char* KeyType;
typedef char* InfoType;
typedef size_t RelType;
typedef size_t IndexType;

typedef struct Table {
	KeySpace *ks;
	IndexType msize;
	IndexType csize;
} Table;

typedef struct KeySpace {
	KeyType *key;
	RelType release;
	InfoType *info;
} KeySpace;

typedef void (*functions)(struct Table*);

Table *init_table(const IndexType);
void free_table(Table*);
table_err insert_element(Table*);
table_err delete_element(Table*);
table_err print_table(const Table*);
table_err import_table_from_file(Table*);
table_err export_table_to_file(const Table*);
table_err search_by_key(const Table*);
table_err search_by_key_with_release(const Table*);
table_err clean_table(Table*);
void exit(Table*);

#endif
