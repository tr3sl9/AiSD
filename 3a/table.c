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

void insert_element(Table * const table) {
//TODO mb enum???, but what mistakes???
	size_t len = 100;
	if (table->csize >= table->msize) {
		printf("Error: Table is full\n");
		return;
	}
	printf("Enter key: ");
	char *key = NULL;
	if (getline(&key, &len, stdin) == -1) {
		printf("Error\n");
		return;
	}
	printf("Enter info: ");
	char *info = NULL;
	if (getline(&info, &len, stdin) == -1) {
		printf("Error\n");
		return;
	}
	RelType release = 1;
	for (IndexType i = 0; i < table->csize; i++) {
		if (strcmp(table->ks[i].key, key) == 0) {
			release++;
		}
	}
	table->ks[table->csize].key = key;
	table->ks[table->csize].release = release;
	table->ks[table->csize].info = info;
	table->csize++;
	printf("Element inserted successfully\n");
	return;
}

void delete_element(Table * const table) {
	size_t len = 100;
    printf("Enter key to delete: ");
    char *key = NULL;
	if (getline(&key, &len, stdin) == -1) {
		printf("Error\n");
		return;
	}
    size_t deleted = 0;
    for (IndexType i = 0; i < table->csize; ) {
        if (strcmp(table->ks[i].key, key) == 0) {
            free(table->ks[i].key);
            free(table->ks[i].info);
            for (IndexType j = i; j < table->csize - 1; j++) {
                table->ks[j] = table->ks[j+1];
            }
            table->csize--;
            deleted++;
        } else {
            i++;
        }
    }
    printf("Deleted %d elements\n", deleted);
	return;
}

void print_table(const Table * const table) {
	printf("Table contents (%zu/%zu):]n", table->csize, table->msize);
	printf("Index\tKey\tRelease\tInfo\n");
	for (IndexType i = 0; i < table->csize; i++) {
		printf("%zu\t%s\t%zu\t%s\n%", i, table->ks[i].key, table->ks[i].release, table->ks[i].info);
	}
	return;
}

void import_table_from_file(Table * const table) {
	size_t len = 100;
	printf("Enter filename: ");
	char *filename = NULL;
	if (getline(&filename, &len, stdin) == -1) {
		printf("Error\n");
		return;
	}
	FILE *file = fopen(filename, "r");
	if (!file) {
		printf("Error oppening file\n");
		return;
	}
	char magic_word[sizeof(MAGIC_WORD)] = {0};
	fgets(magic_word, sizeof(magic_word), file);
	if (strcmp(magic_word, MAGIC_WORD) != 0) {
		fclose(file);
		printf("Error: magic_word\n");
		return;
	}
	size_t msize, csize;
    if (fscanf(file, "%zu %zu\n", &msize, &csize) != 2) {
        fclose(file);
		printf("Error: size\n");
        return;
    }
    if (msize != table->msize) {
        fclose(file);
		printf("Error: size\n");
        return;
    }
	char *line = NULL;
	while (getline(&line, &len, file) != -1) {
		char *key = strtok(line, ":");
		char *release = strtok(NULL, ":")
		char *info = strtok(NULL, "\n");
		if (key && info && release) {
			if (table->csize >= table->msize) {
				printf("Error: Table is full\n");
				fclose(file);
				return;
			}
			table->ks[table->csize].key = key;
			table->ks[table->csize].release = release;
			table->ks[table->csize].info = info;
		}
	}
	fclose(file);
	return;
}

void export_table_to_file(const Table * const table) {
	size_t len = 100;
	if (table == NULL) {
		printf("Error: table is empty\n");
		return;
	}
	printf("Enter filename: ");
	char *filename = NULL;
	if (getline(&filename, &len, stdin) == -1) {
		printf("Error\n");
		return;
	}
	FILE *file = fopen(filename, "w");
	if (!file) {
		printf("Error oppening file\n");
		return;
	}
	fprintf(file, "%s", MAGIC_WORD);
	fprintf(file, "%zu %zu\n", table->msize, table->csize);
	for (IndexType i = 0; i < table->csize; i++) {
		fprintf(file, "%s:%s:$s\n", table->ks[i].key, table->ks[i].release, table->ks[i].info);
	}
	fclose(file);
	return;
}

void search_by_key(const Table * const table) {
	size_t len = 100;
	printf("Enter key: ");
	char *key = NULL;
	if (getline(&key, &len, stdin) == -1) {
		printf("Error\n");
		return;
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
	return;
}

void search_by_key_with_version(const Table * const table) {
	size_t len = 100;
	printf("Enter key: ");
	char *key = NULL;
	if (getline(&key, &len, stdin) == -1) {
		printf("Error\n");
		return;
	}
	printf("Enter version: ");
	RelType version;
	scanf("%zu", &version);
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
	return;
}

void clean_table(Table * const table) {
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
	printf("Table cleaned successfully\n");
	return;
}

void exit(Table * const table) {
	free_table(table);
	printf("Exit\n");
	exit(0);
}
