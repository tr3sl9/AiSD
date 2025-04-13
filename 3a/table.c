#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "table.h"

Table *init_table(const IndexType msize) {
	Table *table = (Table*)malloc(sizeof(Table));
	table->ks = (KeySpace*)malloc(msize * sizeof(KeySpace));
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
	if (table->csize >= table->msize) {
		printf("Error: Table is full\n");
		return;
	}
	printf("Enter key: ");
	char *key = (char*)malloc(100 * sizeof(char));
	fgets(key, sizeof(key), stdin);
	key[strcspn(info, "\n")] = '\0';
	printf("Enter info: ");
	char *info = (char*)malloc(100 * sizeof(char));
	fgets(info, sizeof(info, stdin));
	info[strcspn(info, "\n")] = '\0';
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
    printf("Enter key to delete: ");
    char *key = (char*)malloc(100 * sizeof(char));
    fgets(key, sizeof(key), stdin);
    key[strcspn(key, "\n")] = '\0';
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
	printf("Enter filename: ");
	char *filename = (char*)malloc(100 * sizeof(char));
	fgets(filename, sizeof(filename), stdin);
	filename[strcspn(filename, "\n")] = '\0';
	FILE *file = fopen(filename, "r");
	if (!file) {
		printf("Error oppening file\n");
		return;
	}
	char *line = (char*)malloc(256 * sizeof(char));
	while (fgets(line, sizeof(line), file)) {
		char *key = strtok(line, ":");
		char *info = strtok(NULL, "\n");
		if (key && info) {
			if (table->csize >= table->msize) {
				printf("Error: Table is full\n");
				fclose(file);
				return;
			}
			RelType release = 1;
			for (IndexType i = 0; i < table->csize; i++) {
				if (strcmp(table->ks[i].key, key) == 0) {
						release++;
				}
			}
			table->ks[table->csize].key = key;
			table->ks[table->csize].release = i;
			table->ks[table->csize].info = info;
			table->csize++;
		}
	}
	fclose(file);
	return;
}

void search_by_key(const Table * const table) {
	printf("Enter key: ");
	char *key = (char*)malloc(sizeof(char));
	fgets(key, sizeof(key), stdin);
	key[strcspn(key, "\n")] = '\0';
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
	printf("Enter key: ");
	char *key = (char*)malloc(sizeof(char));
	fgets(key, sizeof(key), stdin);
	key[strcspn(key, "\n")] = '\0';
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
