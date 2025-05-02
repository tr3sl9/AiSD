#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "table.h"

#define MAGIC_WORD "TABLE\n"

void print_ks(const KeySpace * const ks, const size_t i) {
	printf("| %-5zu | %-30s | %-9zu | %-20s |\n", i, ks->key, ks->release, ks->info);
	return;
}

int cmp(const char * const str_1, const char * const str_2) {
	return strcmp(str_1, str_2);
}

int table_initialized(const Table * const table) {
	return table->ks != NULL; 
}

int it_is_not_same_ks(const KeySpace * const ks1, const KeySpace * const ks2) {
	if (ks1 == NULL || ks2 == NULL) return 0;
	return !(cmp(ks1->key, ks2->key) == 0 && ks1->release == ks2->release && cmp(ks1->info, ks2->info) == 0);
}

void set_ks(KeySpace * const ks, const char * const key, const char * const info, const size_t release) {
	if (!ks || !key || !info) return;
	ks->key = strdup(key);
	if (release != 0) ks->release = release;
	ks->info = strdup(info);
	return;
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

void rm_element_with_shift(KeySpace * const current_ks, KeySpace * const last_ks) {
	if (it_is_not_same_ks(current_ks, last_ks)) {
		free_ks(current_ks);
		set_ks(current_ks, last_ks->key, last_ks->info, last_ks->release);
	} 
	free_ks(last_ks);
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
		if (cmp(table->ks[i].key, key) == 0) {
			release++;
		}
	}
	return release;
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

KeySpace **find_elements(const Table * const table, const char * const key, size_t * const count) {
	for (size_t i = 0; i < table->csize; i++) {
		if (cmp(table->ks[i].key, key) == 0) {
			(*count)++;
		}
	}
	KeySpace **key_space_for_res_table = (KeySpace**)calloc(*count, sizeof(KeySpace*));
	if (!key_space_for_res_table) return NULL;
	size_t idx = 0;
	for (size_t i = 0; i < table->csize; i++) {
		if (cmp(table->ks[i].key, key) == 0) {
			key_space_for_res_table[idx] = table->ks + i;
			idx++;
		}
	}
	return key_space_for_res_table;
}

KeySpace *find_element_with_release(const Table * const table, const char * const key, const size_t release) {
	for (size_t i = 0; i < table->csize; i++) {
		if (cmp(table->ks[i].key, key) == 0 && table->ks[i].release == release) {
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
	if (!count) {
		free(ks);
		return TABLE_VAL;
	}
	for (size_t i = count; i > 0; i--) {
		rm_element_with_shift(ks[i - 1], table->ks + table->csize - 1);
		if (table->csize != 0) table->csize--; 
	}
	free(ks);
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
	if (table->csize > 0) {
		rm_element_with_shift(ks, table->ks + table->csize - 1);
		table->csize--;
	}
	return TABLE_OK;
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
	if (cmp(magic_word, MAGIC_WORD) != 0) {
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

size_t find_max_release(const KeySpace * const ks, const size_t count) {
	size_t release = 0;
	for (size_t i = 0; i < count; i++) {
		if (release < ks[i].release) {
			release = ks[i].release;
		}
	}
	return release;
}

table_err clean_table(Table * const table) {
	if (!table || !table_initialized(table)) return TABLE_NULL;
	table_err err = TABLE_OK;
	for (size_t i = 0; i < table->csize; i++) {
		size_t count = 0;
		KeySpace **ks = find_elements(table, table->ks[i].key, &count);
		size_t release = find_max_release(*ks, count);
		for (size_t j = count; j > 0; j--) {
			if (release > ks[j - 1]->release) {
				rm_element_with_shift(ks[j - 1], table->ks + table->csize - 1);
				if (table->csize != 0) table->csize--;
				i--;
			}
		}
		free(ks);
	}
	return err;
}
