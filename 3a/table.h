#ifndef TABLE_H
#define TABLE_H

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
void insert_element(Table*);
void delete_element(Table*);
void print_table(const Table*);
void import_table_from_file(Table*);
void export_table_to_file(const Table*);
void search_by_key(const Table*);
void search_by_key_with_release(const Table*);
void clean_table(Table*);
void exit(Table*);

#endif
