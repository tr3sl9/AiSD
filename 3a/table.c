#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "table.h"

#define MAGIC_WORD "TABLE\n"

int table_initialized(const Table * const table) {
	return table->ks != NULL; 
} 

Table *create_table(const size_t msize) {
	Table *table = (Table*)calloc(1, sizeof(Table));
	if (table == NULL) {
		return NULL;
	}
	table->ks = (KeySpace*)calloc(msize, sizeof(KeySpace));
	if (table->ks == NULL) {
		free(table);
		return NULL;
	}
	table->msize = msize;
	table->csize = 0;
	return table;
}

void free_ks(KeySpace * const ks) {
	if (!ks) {
		return;
	}
	free(ks->key);
	free(ks->info);
	return;
}

void rm_element(KeySpace * const ks) {
	ks->key = NULL;
	return;
}

void free_table(Table * const table) {
	for (size_t i = 0; i < table->csize; i++) {
		free_ks(table->ks + i);
	}
	free(table->ks);
	free(table);
	return;
}

size_t find_last_release(const Table * const table, const char * const key) {
	size_t release = 0;
	for (size_t i = 0; i < table->csize; i++) {
		if (strcmp(table->ks[i].key, key) == 0) {
			release++;
		}
	}
	return release;
}

void set_ks(KeySpace * const ks, const char * const key, const char * const info, const size_t release) {
	if (!ks || !key || !info) return;
	ks->key = strdup(key);
	if (release != 0) ks->release = release;
	ks->info = strdup(info);
	return;
}

table_err insert_key_to_table(Table * const table, const char * const key, const char * const info) {
	if (!table) return TABLE_NULL;
	if (!key || !info) return TABLE_VAL;
	if (table->csize >= table->msize) return TABLE_FULL;	
	size_t release = find_last_release(table, key);
	set_ks(table->ks + table->csize, key, info, release + 1);
	table->csize++;
	return TABLE_OK;
}

void shifting_elements_from_table(Table * const table) {
	size_t j = 0;
	for (size_t i = 0; i < table->msize; i++) {
		if (table->ks[j].key == NULL && table->ks[i].key != NULL) {
			if (i != j) {
				set_ks(table->ks + j, table->ks[i].key, table->ks[i].info, table->ks[i].release);
				rm_element(table->ks + i);
				j++; 
			}
		}
	}
	return;
}

KeySpace **find_elements(const Table * const table, const char * const key, size_t * const count) {
	for (size_t i = 0; i < table->csize; i++) {
		if (strcmp(table->ks[i].key, key) == 0) {
			(*count)++;
		}
	}
	KeySpace **key_space_for_res_table = (KeySpace**)calloc(*count, sizeof(KeySpace*));
	if (!key_space_for_res_table) return NULL;
	size_t idx = 0;
	for (size_t i = 0; i < table->csize; i++) {
		if (strcmp(table->ks[i].key, key) == 0) {
			key_space_for_res_table[idx] = table->ks + i;
			idx++;
		}
	}
	return key_space_for_res_table;
}

KeySpace *find_element_with_release(const Table * const table, const char * const key, const size_t release) {
	for (size_t i = 0; i < table->csize; i++) {
		if (strcmp(table->ks[i].key, key) == 0 && table->ks[i].release == release) {
			return table->ks + i;
		}
	}
	return NULL;
}

table_err delete_key_from_table(Table * const table, const char * const key) {
	if (!table) {
		return TABLE_NULL;
	}
	if (!key) {
		return TABLE_VAL;
	}
	size_t count = 0;
	KeySpace **ks = find_elements(table, key, &count);
	if (!ks) {
		return TABLE_VAL;
	}
	for (size_t i = 0; i < count; i++) {
		rm_element(ks[i]);
		if (table->csize != 0) table->csize--; 
	}
	free(ks);
	shifting_elements_from_table(table); 
	return TABLE_OK;
}

table_err delete_element_with_release(Table * const table, const char * const key, const size_t release) {
	if (!table) {
		return TABLE_NULL;
	}
	if (!key) {
		return TABLE_VAL;
	}
	KeySpace *ks = find_element_with_release(table, key, release);
	if (!ks) {
		return TABLE_VAL;
	}
	rm_element(ks);
    if (table->csize != 0) table->csize--;
	shifting_elements_from_table(table);
	return TABLE_OK;
}

void print_ks(const KeySpace * const ks, const size_t i) {
	printf("| %-5zu | %-30s | %-9zu | %-20s |\n", i, ks->key, ks->release, ks->info);
	return;
}

table_err print_table(const Table * const table) {
    if (!table) return TABLE_NULL;
    printf("\nTable contents (%zu/%zu):\n", table->csize, table->msize);
    printf("┌───────┬────────────────────────────────┬───────────┬──────────────────────┐\n");
    printf("│ Index │ Key                            │ Release   │ Info                 │\n");
    printf("├───────┼────────────────────────────────┼───────────┼──────────────────────┤\n");
    for (size_t i = 0; i < table->csize; i++) {
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
    if (csize > msize) {
        fclose(file);
        return TABLE_SIZE;
    }
	size_t len = 100;
	char *line = NULL;
	while (table->csize <= table->msize && getline(&line, &len, file) != -1) {
		char* key = strtok(line, ":");
		char *release = strtok(NULL, ":");
		char* info = strtok(NULL, "\n");
		if (key && info && release) {
			if (table->csize >= table->msize) {
				fclose(file);
				return TABLE_FULL;
			}
			set_ks(table->ks + table->csize, key, info, atoi(release));
			table->csize++;
		}
	}
	free(line);
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
	for (size_t i = 0; i < table->csize; i++) {
		fprintf(file, "%s:%zu:%s\n", table->ks[i].key, table->ks[i].release, table->ks[i].info);
	}
	fclose(file);
	return TABLE_OK;
}

Table* search_by_key_in_table(const Table * const table, const char * const key) {
	if (!table || !key || !table_initialized(table)) {
		return NULL;
	}
	size_t count = 0;
	KeySpace **ks = find_elements(table, key, &count);
	if (!ks) {
		return NULL;
	} 
	Table *result = create_table(count);
	if (!result) {
		return NULL;
	}
	for (size_t i = 0; i < result->msize; i++) {
		set_ks(result->ks + result->csize, ks[i]->key, ks[i]->info, ks[i]->release);
		result->csize++;
	}
	free(ks);
	return result;
}

Table* search_by_key_with_release_in_table(const Table * const table, const char * const key, const size_t release) {
	if (!table || !key || !table_initialized(table)) {
		return NULL;
	}
	Table *result = create_table(1);
	if (!result) {
		return NULL;
	}
	KeySpace *ks = find_element_with_release(table, key, release);
	if (!(result->ks)) {
		return NULL;
	}
	set_ks(result->ks, ks->key, ks->info, ks->release);
	result->csize++;
	return result;
}

size_t find_max_release(const Table * const table) {
	size_t release = 0;
	for (size_t i = 0; i < table->csize; i++) {
		if (release < table->ks[i].release) {
			release = table->ks[i].release;
		}
	}
	return release;
}

table_err clean_table(Table * const table) {
	if (!table || !table_initialized(table)) return TABLE_NULL;
	table_err err = TABLE_OK;
	for (size_t i = 0; i < table->csize; i++) {
		Table *result = search_by_key_in_table(table, table->ks[i].key);
		if (!result) continue;
		size_t release = find_max_release(result);
		for (size_t j = 0; i < result->csize; j++) {
			if (result->ks[j].release < release) {
				err = delete_element_with_release(table, table->ks[i].key, result->ks[j].release);
			}
		}
		shifting_elements_from_table(table);
		free_table(result);
		if (err != TABLE_OK) return err;
	}
	return err;
}
