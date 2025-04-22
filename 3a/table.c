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
		init_key_space(table->ks + i);
	}
	table->msize = msize;
	table->csize = 0;
	return table;
}

void free_table(Table * const table) {
	for (IndexType i = 0; i < table->csize; i++) {
		free(table->ks[i].key);
		free(table->ks[i].info);
	}
	free(table->ks);
	free(table);
	return;
}

RelType find_last_release(const Table * const table, const KeyType key) {
	RelType release = 1;
	for (IndexType i = 0; i < table->csize; i++) {
		if (strcmp(*(table->ks[i].key), key) == 0) {
			release++;
		}
	}
	return release;
}

void set_key(KeySpace * const ks, const KeyType key, const InfoType info, const RelType release) {
	if (!ks || !key || !info) return;
	*(ks->key) = strdup(key);
	if (release != 0) ks->release = release;
	*(ks->info) = strdup(info);
	return;
}

void init_key_space(KeySpace *ks) {
	if (!ks) return;
	ks->key = (KeyType*)calloc(1, sizeof(KeyType));
	ks->info = (InfoType*)calloc(1, sizeof(InfoType));
	ks->release = 0;
	return;
}

table_err insert_element(Table * const table, const KeyType key, const InfoType info) {
	if (!table) return TABLE_NULL;
	if (!key || !info) return TABLE_VAL;
	if (table->csize >= table->msize) return TABLE_FULL;	
	RelType release = find_last_release(table, key);
	set_key(table->ks + table->csize, key, info, release + 1);
	table->csize++;
	return TABLE_OK;
}

void free_ks(KeySpace * const ks) {
	if (!ks) {
		return;
	}
	if (ks->key) {
		free(*(ks->key));
		free(ks->key);
	}
	if (ks->info) {
		free(*(ks->info));
		free(ks->info);
	}
	return;
}

table_err delete_element(Table * const table, const KeyType key) {
	if (!table) {
		return TABLE_NULL;
	}
	if (!key) {
		return TABLE_VAL;
	}
    for (IndexType i = 0; i < table->csize; i++) {
        if (strcmp(*(table->ks[i].key), key) == 0) {
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
        if (strcmp(*(table->ks[i].key), key) == 0 && table->ks[i].release < release) {
            free_ks(table->ks + i);
            table->csize--;
        }
    }
	return TABLE_OK;
}

void print_ks(const KeySpace * const ks, size_t i) {
	printf("| %-5zu | %-30s | %-9zu | %-20s |\n", i, *(ks->key), ks->release, *(ks->info));
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
			set_key(table->ks + csize, key, info, (RelType)release);
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
		fprintf(file, "%s:%zu:%s\n", *(table->ks[i].key), table->ks[i].release, *(table->ks[i].info));
	}
	fclose(file);
	return TABLE_OK;
}

Table* search_by_key(Table * const table, const KeyType key) {
	if (!table) {
		return NULL;
	}
	if (!key) {
		return NULL;
	}
	Table *result = create_table();
	result = init_table(table, 1);
	if (!result) {
		return NULL;
	}
	for (IndexType i = 0; i < table->csize; i++) {
		if (strcmp(*(table->ks[i].key), key) == 0) {
			if (result->msize == result->csize) {
				result->ks = (KeySpace*)realloc(result->ks, ((result->msize) + 1) * sizeof(KeySpace));
				result->msize++;
			}
			set_key(table->ks + table->csize, key, *(table->ks[i].info), 0);
			result->csize++;
			break;
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
		if (strcmp(*(table->ks[i].key), key) == 0 && table->ks[i].release == release) {
			if (result->msize == result->csize) {
				result->ks = (KeySpace*)realloc(result->ks, ((result->msize) + 1) * sizeof(KeySpace));
				result->msize++;
			}
			set_key(table->ks + table->csize, key, *(table->ks[i].info), release);
			result->csize++;
			break;
		}
	}
	return result;
}

table_err clean_table(Table* table) {
	if (!table) return TABLE_NULL;
	for (IndexType i = 0; i < table->csize; i++) {
		Table *result = search_by_key(table, *(table->ks[i].key));
		IndexType release = 0;
		for (IndexType j = 0; j < result->csize; j++) {
			if (release < result->ks[j].release) {
				release = result->ks[i].release;
			}
		}
		free_table(result);
		table_err err = delete_element_with_release(table, *(table->ks[i].key), release);
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
