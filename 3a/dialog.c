#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "table.h"
#include "dialog.h"

KeyType read_key() {
	size_t len = 100;
	KeyType key = NULL;
	while (1) {
		printf("Enter key: ");
		if (getline(&key, &len, stdin) == -1) {
			free(key);
			return NULL;
		} else {
			break;
		}
	}
	key[strcspn(key, "\n")] = '\0';
	return key;
}

InfoType read_info() {
	size_t len = 100;
	InfoType info = NULL;
	while (1) {
		printf("Enter info: ");
		if (getline(&info, &len, stdin) == -1) {
			free(info);
			return NULL;
		} else {
			break;
		}
	}
	info[strcspn(info, "\n")] = '\0';
	return info;
}

RelType read_number() {
	RelType number;
	while(1) {
		printf("Enter number (from 0 to infinity): ");
		if (scanf("%zu", &number) == EOF) {
			return EOF;
		} 
		else if (number <= 0) {
			printf("Error: Value\nTry again\n");
		}else {
			break;
		}
	}
	return number;
}

table_err dialog_insert(Table * const table) {
	if (table->csize >= table->msize) {
		return TABLE_FULL;
	}
	KeyType key = read_key();
	if (!key) {
		free(key);
		return TABLE_EOF;
	}
	InfoType info = read_info();
	if (!info) {
		return TABLE_EOF;
	}
	table_err err = insert_key_to_table(table, key, info);
	free(key);
	free(info);
	return err;
}

table_err dialog_delete(Table * const table) {
	if (table->csize == 0) {
		return TABLE_NULL;
	}
	KeyType key = read_key();
	if (!key) {
		return TABLE_EOF;
	}
	table_err err = delete_key_from_table(table, key);
	free(key);
	return err;
}

table_err dialog_print(Table * const table) {
	return print_table(table);
}

table_err dialog_find(Table * const table) {
	KeyType key = read_key();
	if (!key) {
		return TABLE_EOF;
	}
	Table *result = search_by_key_in_table(table, key);
	if (!result) {
		return TABLE_NULL;
	}
	print_table(result);
	free_table(result);
	free(key);
	return TABLE_OK;
}

table_err dialog_find_release(Table * const table) {
	KeyType key = read_key();
	if (!key) {
		return TABLE_EOF;
	}
	RelType release = read_number();
	if (release == (RelType)(EOF)) {
		return TABLE_EOF;
	}
	Table *result = search_by_key_with_release_in_table(table, key, release);
	if (!result || result->csize == 0) {
		return TABLE_SIZE;
	}
	print_table(result);
	free_table(result);
	free(key);
	return TABLE_OK;
}

table_err dialog_import(Table * const table) {
	char *filename = read_info();
	if (!filename) {
		return FILE_ERR;
	} 
	return import_table_from_file(table, filename);
}

table_err dialog_clean(Table * const table) {
	return clean_table(table);
}

table_err dialog_export(Table * const table) {
	char *filename = read_info();
	if (!filename) {
		return FILE_ERR;
	}
	return export_table_to_file(table, filename);
}

table_err dialog_exit(Table * const table) {
	return TABLE_EXIT;
}

const functions operation[] = {
	dialog_insert,
	dialog_delete,
	dialog_find,
	dialog_find_release,
	dialog_print,
	dialog_import,
	dialog_export,
	dialog_clean,
	dialog_exit
};

const char *menu_items[COUNT_OP] = {
	"Insert element",
	"Delete by key",
	"Search by key",
	"Search by key and version",
	"Print table",
	"Import from file",
	"Export to file",
	"Clean table",
	"EXIT"
};

void show_menu() {
	for (size_t i = 0; i < COUNT_OP; i++) {
		printf("[%zu]: %s\n", i + 1, menu_items[i]);
	}
	return;
}

int process_choice(Table *table, size_t choice) {
	if (choice < 1 || choice > COUNT_OP) {
		printf("Invalid choice\n");
		return 0;
	}
	table_err err = operation[choice - 1](table);
	switch (err) {
        case TABLE_OK: printf("ALL'S OKAY\n"); break;
		case TABLE_EMPTY: printf("Error: Table is empty\n"); break;
		case TABLE_FULL: printf("Error: Table is full\n"); break;
		case TABLE_MEM: printf("Error: Memory allocation failed\n"); break;
		case TABLE_NULL: printf("Error: Table is null\n"); break;
		case TABLE_VAL: printf("Error: value\n"); break;
		case TABLE_SIZE: printf("Error: Table size mismatch\n"); break;
		case TABLE_MAGIC_WORD: printf("Error: Invalid file format\n"); break;
        case FILE_ERR: printf("Error: Cannot open file\n"); break;
		case TABLE_EOF: printf("Error: EOF\n"); break;					   
        case TABLE_EXIT: printf("EXIT\n"); break;
        default: printf("Unknown error\n");
    }
	if (err == TABLE_EOF || err == TABLE_EXIT || err == FILE_ERR) return 1;
	return 0;
}
