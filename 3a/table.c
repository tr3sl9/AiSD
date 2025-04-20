#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "table.h"

#define MAGIC_WORD "TABLE\n"

Table *init_table(const IndexType msize) {
	Table *table = (Table*)malloc(sizeof(Table));
	if (table == NULL) {
		return NULL;
	}
	table->ks = (KeySpace*)malloc(msize * sizeof(KeySpace));
	if (table->ks == NULL) {
		free(table);
		return NULL;
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
	ks->release = release;
	*(ks->info) = strdup(info);
	return;
}

table_err insert_element(Table * const table, const KeyType key, const InfoType info) {
	if (!table) {
		return TABLE_NULL;
	}
	if (!key || !info) {
		return TABLE_VAL;
	}
	RelType release = find_last_release(table, key);
	set_key(table->ks + table->csize, key, info, release);
	table->csize++;
	return TABLE_OK;
}

void free_ks(KeySpace * const ks) {
	free(ks->key);
	free(ks->info);
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

void print_ks(const KeySpace * const ks, size_t i) {
	printf("%zu\t%s\t%zu\t%s\n", i, *(ks->key), ks->release, *(ks->info));
	return;
}

table_err print_table(const Table * const table) {
	if (!table) {
		return TABLE_NULL;
	}
	printf("Table contents (%zu/%zu):]n", table->csize, table->msize);
	printf("Index\tKey\tRelease\tInfo\n");
	for (IndexType i = 0; i < table->csize; i++) {
		print_ks(table->ks + i, i);
	}
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

table_err search_by_key(const Table * const table, const char * const key) {
	if (!table) {
		return TABLE_NULL;
	}
	if (!key) {
		return TABLE_VAL;
	}
	Table *result = init_table(1);
	if (!result) {
		return TABLE_NULL;
	}
	for (IndexType i = 0; i < table->csize; i++) {
		if (strcmp(*(table->ks[i].key), key) == 0) {
			if (result->msize == result->csize) {
				result->ks = (KeySpace*)realloc(result->ks, ((result->msize) + 1) * sizeof(KeySpace));
				result->msize++;
			}
			set_key(table->ks + table->csize, key, info, release);
			result->csize++;
			break;
		}
	}
	printf("Search results:\n");
	print_table(result);
	free_table(result);
	return TABLE_OK;
}

table_err search_by_key_with_version(const Table * const table, const char * const key, const RelType release) {
	if (!table) {
		return TABLE_NULL;
	}
	if (!key || !release) {
		return TABLE_VAL;
	}
	Table *result = init_table(1);
	if (!result) {
		return TABLE_NULL;
	}
	for (IndexType i = 0; i < table->csize; i++) {
		if (strcmp(table->ks[i].key, key) == 0 && table->ks[i].release == release) {
			if (result->msize == result->csize) {
				result->ks = (KeySpace*)realloc((result->msize) + 1, sizeof(KeySpace));
				table->msize++;
			}
			set_key(table->ks + csize, key, info, release);
			result->csize++;
			break;
		}
	}
	printf("Search results:\n");
	print_table(result);
	free_table(result);
	return TABLE_OK;
}

void table_realloc(Table * const table) {
	table->ks = (KeySpace*)	
}

void clean_table(Table * const table) {
	if (table == NULL) {
		return TABLE_NULL;
	}
	for (IndexType i = 0; i < table->csize; ) {
		KeyType ckey = table->ks[i].key;
		RelType mrelease = 0;
		IndexType lindex = i;
		for (IndexType j = i; j < table->csize; j++) {
			if (strcmp(table->ks[j].key, ckey) == 0) {
				if (table->ks[j].release > mrelease) {
					mrelease = table->ks[j].release;
					lindex = j;
				}
			} else {
				break;
			}
		}
		for (IndexType j = i; j < table->csize; ) {
			if (strcmp(table->ks[j].key, ckey) == 0 && table->ks[j].release != mrelease) {
				free(table->ks[j].key);
				free(table->ks[j].info);
				for (IndexType k = j; k < table->csize - 1; k++) {
					table->ks[k] = table->ks[k + 1];
					table->msize++;
				}
				table->csize--;
			} else {
				j++;
			}  
		}
		i++;
	}
	return TABLE_OK;
}

void exit_from_prog(Table * const table) {
	free_table(table);
	printf("Exit\n");
	exit(0);
}
