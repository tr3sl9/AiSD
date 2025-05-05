#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include "table.h"
#include "dialog.h"

#define PROMPT_FOR_KEY "Enter key: "
#define PROMPT_FOR_INFO "Enter info: "

char* read_key() {
	char* key = NULL;
	while (1) {
		key = readline(PROMPT_FOR_KEY);
		if (key == NULL) {
			free(key);
			return NULL;
		} else {
			break;
		}
	}
	return key;
}

char* read_info() {
	char* info = NULL;
	while (1) {
		info = readline(PROMPT_FOR_INFO);
		if (info == NULL) {
			free(info);
			return NULL;
		} else {
			break;
		}
	}
	return info;
}

size_t read_number() {
	size_t number;
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
	char* key = read_key();
	if (!key) {
		free(key);
		return TABLE_EOF;
	}
	char* info = read_info();
	if (!info) {
		free(key);
		free(info);
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
	char* key = read_key();
	if (!key) {
		free(key);
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
	if (table->csize == 0) {
		return TABLE_NULL;
	}
	char* key = read_key();
	if (!key) {
		free(key);
		return TABLE_EOF;
	}
	Table *result = search_by_key_in_table(table, key);
	if (!result) {
		free(key);
		return TABLE_NULL;
	}
	print_table(result);
	free_table(result);
	free(key);
	return TABLE_OK;
}

table_err dialog_find_release(Table * const table) {
	if (table->csize == 0) {
		return TABLE_NULL;
	}
	char* key = read_key();
	if (!key) {
		free(key);
		return TABLE_EOF;
	}
	size_t release = read_number();
	if (release == (size_t)(EOF)) {
		free(key);
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
		free(filename);
		return FILE_ERR;
	}
	table_err err = import_table_from_file(table, filename);
	free(filename);
	return err;
}

table_err dialog_clean(Table * const table) {
	return clean_table(table);
}

table_err dialog_export(Table * const table) {
	if (table->csize != 0) {
		return TABLE_SIZE;
	}
	char *filename = read_info();
	if (!filename) {
		free(filename);
		return FILE_ERR;
	}
	table_err err = export_table_to_file(table, filename);
	free(filename);
	return err;
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
	table_err err = TABLE_OK;
	if (strcmp(menu_items[choice - 1], "EXIT") == 0) {
		err = TABLE_EXIT;
	} else {
		err = operation[choice - 1](table);
	}
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
	if (err == TABLE_EOF || err == TABLE_EXIT) return 1;
	return 0;
}
