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
	if (table == NULL) {
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

table_err insert_element(Table * const table, const KeyType const key, const InfoType const info) {
	if (!table) {
		return TABLE_NULL;
	}
	if (!key || !info) {
		return TABLE_VAL;
	}
	RelType release = 1;
	for (IndexType i = 0; i < table->csize; i++) {
		if (strcmp(table->ks[i].key, key) == 0) {
			release++;
		}
	}
	table->ks[table->csize].key = strdup(key);
	table->ks[table->csize].release = release;
	table->ks[table->csize].info = strdup(info);
	table->csize++;
	return TABLE_OK;
}

table_err delete_element(Table * const table, const KeyType const key) {
	if (!table) {
		return TABLE_NULL;
	}
	if (!key) {
		return TABLE_VAL;
	}
    for (IndexType i = 0; i < table->csize; ) {
        if (strcmp(table->ks[i].key, key) == 0) {
            free(table->ks[i].key);
            free(table->ks[i].info);
            for (IndexType j = i; j < table->csize - 1; j++) {
                table->ks[j] = table->ks[j+1];
            }
            table->csize--;
        } else {
            i++;
        }
    }
	return TABLE_OK;
}

table_err print_table(const Table * const table) {
	if (!table) {
		return TABLE_NULL;
	}
	printf("Table contents (%zu/%zu):]n", table->csize, table->msize);
	printf("Index\tKey\tRelease\tInfo\n");
	for (IndexType i = 0; i < table->csize; i++) {
		printf("%zu\t%s\t%zu\t%s\n%", i, table->ks[i].key, table->ks[i].release, table->ks[i].info);
	}
	return TABLE_OK;
}

table_err import_table_from_file(Table * const table, const chat * const filename) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		return FILE_ERR;
	}
	char magic_word[sizeof(MAGIC_WORD)] = {0};
	fgets(magic_word, sizeof(magic_word), file);
	if (strcmp(magic_word, MAGIC_WORD) != 0) {
		fclose(file);
		return TABLE_MAGIC_WRORD;
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
	char *line = NULL;
	while (getline(&line, &len, file) != -1) {
		char *key = strtok(line, ":");
		char *release = strtok(NULL, ":")
		char *info = strtok(NULL, "\n");
		if (key && info && release) {
			if (table->csize >= table->msize) {
				fclose(file);
				return TABLE_FULL;
			}
			table->ks[table->csize].key = key;
			table->ks[table->csize].release = release;
			table->ks[table->csize].info = info;
		}
	}
	fclose(file);
	return TBALE_OK;
}

table_err export_table_to_file(const Table * const table, const char * const filename) {
	FILE *file = fopen(filename, "w");
	if (!file) {
		return FILE_ERR;
	}
	fprintf(file, "%s", MAGIC_WORD);
	fprintf(file, "%zu %zu\n", table->msize, table->csize);
	for (IndexType i = 0; i < table->csize; i++) {
		fprintf(file, "%s:%s:$s\n", table->ks[i].key, table->ks[i].release, table->ks[i].info);
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
	Table *result = init_table(table->msize);
	if (!result) {
		return TABLE_NULL;
	}
	for (IndexType i = 0; i < table->csize; i++) {
		if (strcmp(table->ks[i].key, key) == 0 && table->ks[i].release == version) {
			result->ks[result->csize].key = key;
			result->ks[result->csize].info = info;
			result->ks[result->csize].release = table->ks[i].release;
			result->csize++;
			break;
		}
	}
	printf("Search results:\n");
	print_table(result);
	free_table(result);
	return TABLE_OK;
}

table_err search_by_key_with_version(const Table * const table, const char * const key, const version) {
	if (!table) {
		return TABLE_NULL;
	}
	if (!key || !version) {
		return TABLE_VAL;
	}
	Table *result = init_table(table->msize);
	for (IndexType i = 0; i < table->csize; i++) {
		if (strcmp(table->ks[i].key, key) == 0 && table->ks[i].release == version) {
			result->ks[result->csize].key = key;
			result->ks[result->csize].info = info;
			result->ks[result->csize].release = table->ks[i].release;
			result->csize++;
			break;
		}
	}
	printf("Search results:\n");
	print_table(result);
	free_table(result);
	return TABLE_OK;
}

table_err clean_table(Table * const table) {
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

void exit(Table * const table) {
	free_table(table);
	printf("Exit\n");
	exit(0);
}
