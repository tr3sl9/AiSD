#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include "table.h"

#define MAGIC_WORD "TABLE\n"

int table_initialized(Table *table) {
	return table->ks != NULL; 
} 

Table *create_table() {
	Table *table = (Table*)calloc(1, sizeof(Table));
	return table;
}

void init_ks(KeySpace * const ks) {
	if (!ks) return;
	ks->key = (KeyType)calloc(1, sizeof(char));
	ks->info = (InfoType)calloc(1, sizeof(char));
	ks->release = 0;
	return;
}

Table *init_table(Table * const table, const IndexType msize) {
	if (table == NULL) {
		return NULL;
	}
	table->ks = (KeySpace*)calloc(msize, sizeof(KeySpace));
	if (table->ks == NULL) {
		free(table);
		return NULL;
	}
	for (IndexType i = 0; i < msize; i++) {
		init_ks(table->ks + i);
	}
	table->msize = msize;
	table->csize = 0;
	return table;
}

void free_ks(KeySpace * const ks) {
	if (!ks) {
		return;
	}
	if (ks->key) {
		free(ks->key);
	}
	if (ks->info) {
		free(ks->info);
	}
	return;
}

void free_table(Table * const table) {
	for (IndexType i = 0; i < table->csize; i++) {
		free_ks(table->ks + i);
	}
	free(table->ks);
	free(table);
	return;
}

RelType find_last_release(const Table * const table, const KeyType key) {
	RelType release = 0;
	for (IndexType i = 0; i < table->csize; i++) {
		if (strcmp(table->ks[i].key, key) == 0) {
			release++;
		}
	}
	return release;
}

void set_ks(KeySpace * const ks, const KeyType key, const InfoType info, const RelType release) {
	if (!ks || !key || !info) return;
	ks->key = strdup(key);
	if (release != 0) ks->release = release;
	ks->info = strdup(info);
	return;
}

table_err insert_element(Table * const table, const KeyType key, const InfoType info) {
	if (!table) return TABLE_NULL;
	if (!key || !info) return TABLE_VAL;
	if (table->csize >= table->msize) return TABLE_FULL;	
	RelType release = find_last_release(table, key);
	set_ks(table->ks + table->csize, key, info, release + 1);
	table->csize++;
	return TABLE_OK;
}

table_err delete_element(Table * const table, const KeyType key) {
	if (!table) {
		return TABLE_NULL;
	}
	if (!key) {
		return TABLE_VAL;
	}
    for (IndexType i = 0; i < table->csize; i++) {
        if (strcmp(table->ks[i].key, key) == 0) {
            free_ks(table->ks + i);
            table->csize--;
        }
    }
	return TABLE_OK;
}

table_err delete_element_with_release(Table * const table, const KeyType key, const RelType release) {
	if (!table) {
		return TABLE_NULL;
	}
	if (!key) {
		return TABLE_VAL;
	}
    for (IndexType i = 0; i < table->csize; i++) {
        if (strcmp(table->ks[i].key, key) == 0 && table->ks[i].release < release) {
            free_ks(table->ks + i);
            table->csize--;
        }
    }
	return TABLE_OK;
}

void print_ks(const KeySpace * const ks, size_t i) {
	printf("| %-5zu | %-30s | %-9zu | %-20s |\n", i, ks->key, ks->release, ks->info);
	return;
}

table_err print_table(const Table * const table) {
    if (!table) return TABLE_NULL;
    printf("\nTable contents (%zu/%zu):\n", table->csize, table->msize);
    printf("┌───────┬────────────────────────────────┬───────────┬──────────────────────┐\n");
    printf("│ Index │ Key                            │ Release   │ Info                 │\n");
    printf("├───────┼────────────────────────────────┼───────────┼──────────────────────┤\n");
    for (IndexType i = 0; i < table->csize; i++) {
        print_ks(table->ks + i, i);
        if (i < table->csize - 1) {
            printf("├───────┼────────────────────────────────┼───────────┼──────────────────────┤\n");
        }
    }
    printf("└───────┴────────────────────────────────┴───────────┴──────────────────────┘\n");
    return TABLE_OK;
}

table_err import_table_from_file(Table * const table, const char * const filename) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		return FILE_ERR;
	}
	char magic_word[sizeof(MAGIC_WORD)] = {0};
	fgets(magic_word, sizeof(magic_word), file);
	if (strcmp(magic_word, MAGIC_WORD) != 0) {
		fclose(file);
		return TABLE_MAGIC_WORD;
	}
	size_t msize, csize;
    if (fscanf(file, "%zu %zu\n", &msize, &csize) != 2) {
        fclose(file);
        return TABLE_SIZE;
    }
    if (msize != table->msize) {
        fclose(file);
        return TABLE_SIZE;
    }
	size_t len = 100;
	char *line = NULL;
	while (getline(&line, &len, file) != -1) {
		KeyType key = strtok(line, ":");
		char *release = strtok(NULL, ":");
		InfoType info = strtok(NULL, "\n");
		if (key && info && release) {
			if (table->csize >= table->msize) {
				fclose(file);
				return TABLE_FULL;
			}
			set_ks(table->ks + csize, key, info, (RelType)release);
		}
	}
	fclose(file);
	return TABLE_OK;
}

table_err export_table_to_file(const Table * const table, const char * const filename) {
	FILE *file = fopen(filename, "w");
	if (!file) {
		return FILE_ERR;
	}
	fprintf(file, "%s", MAGIC_WORD);
	fprintf(file, "%zu %zu\n", table->msize, table->csize);
	for (IndexType i = 0; i < table->csize; i++) {
		fprintf(file, "%s:%zu:%s\n", table->ks[i].key, table->ks[i].release, table->ks[i].info);
	}
	fclose(file);
	return TABLE_OK;
}

Table* search_by_key(Table * const table, const KeyType key) {
	if (!table || !key) {
		return NULL;
	}
	Table *result = create_table();
	if (!table) {
		return NULL;
	}
	result = init_table(table, 0);
	if (!result) {
		free(result);
		return NULL;
	}
	for (IndexType i = 0; i < table->csize; i++) {
		if (strcmp(table->ks[i].key, key) == 0) {
				result->ks = (KeySpace*)realloc(result->ks, ((result->msize) + 1) * sizeof(KeySpace));
				if (!result->ks) {
					free_table(result);
					return NULL;
				}
				init_ks(&result->ks[result->msize]);
				set_ks(&result->ks[result->msize], table->ks[i].key, table->ks[i].info, table->ks[i].release);
				result->msize++;
			result->csize++;
		}
	}
	return result;
}

Table* search_by_key_with_release(Table * const table, const KeyType key, const RelType release) {
	if (!table) {
		return NULL;
	}
	if (!key || !release) {
		return NULL;
	}
	Table *result = create_table();
	result = init_table(table, 1);
	if (!result) {
		return NULL;
	}
	for (IndexType i = 0; i < table->csize; i++) {
		if (strcmp(table->ks[i].key, key) == 0 && table->ks[i].release == release) {
			if (result->msize == result->csize) {
				result->ks = (KeySpace*)realloc(result->ks, ((result->msize) + 1) * sizeof(KeySpace));
				result->msize++;
			}
			set_ks(table->ks + table->csize, key, table->ks[i].info, release);
			result->csize++;
			break;
		}
	}
	return result;
}

RelType find_release(Table * const table) {
	IndexType release = 0;
	for (IndexType i = 0; i < table->csize; i++) {
		if (release < table->ks[i].release) {
			release = table->ks[i].release;
		}
	}
	return release;
}

table_err clean_table(Table* table) {
	if (!table) return TABLE_NULL;
	for (IndexType i = 0; i < table->csize; i++) {
		Table *result = search_by_key(table, table->ks[i].key);
		RelType release = find_release(result);
		free_table(result);
		table_err err = delete_element_with_release(table, table->ks[i].key, release);
		if (err == TABLE_NULL) return TABLE_NULL;
		if (err == TABLE_VAL) return TABLE_VAL;
	}
	return TABLE_OK;
}

void exit_from_prog(Table * const table) {
	free_table(table);
	printf("Exit\n");
	exit(0);
}
