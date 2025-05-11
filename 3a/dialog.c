#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <readline/readline.h>
#include "table.h"
#include "dialog.h"
#include "testing.h"

#define PROMPT_FOR_KEY "Enter key: "
#define PROMPT_FOR_INFO "Enter info: "
#define COUNT_OP sizeof(operations) / sizeof(operation)

static char* read_key() {
	char* key = NULL;
	while (!key) {
		key = readline(PROMPT_FOR_KEY);
		if (key == NULL) {
			return NULL;
		} 
	}
	return key;
}

static char* read_info() {
	char* info = NULL;
	while (!info) {
		info = readline(PROMPT_FOR_INFO);
		if (info == NULL) {
			return NULL;
		} 
	}
	return info;
}

static table_err dialog_insert(Table * const table) {
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

static table_err dialog_delete(Table * const table) {
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

static table_err dialog_print(Table * const table) {
	return print_table(table);
}

static table_err dialog_find(Table * const table) {
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

static table_err dialog_find_release(Table * const table) {
	if (table->csize == 0) {
		return TABLE_NULL;
	}
	char* key = read_key();
	if (!key) {
		free(key);
		return TABLE_EOF;
	}
	size_t release;
	if (read_number(&release, 0, INT_MAX) == TABLE_EOF) {
		free(key);
		return TABLE_EOF;
	}
	Table *result = search_by_key_with_release_in_table(table, key, release);
	if (!result || result->csize == 0) {
		return TABLE_VAL;
	}
	print_table(result);
	free_table(result);
	free(key);
	return TABLE_OK;
}

static table_err dialog_import(Table * const table) {
	char *filename = read_info();
	if (!filename) {
		free(filename);
		return FILE_ERR;
	}
	table_err err = import_table_from_file(table, filename);
	free(filename);
	return err;
}

static table_err dialog_clean(Table * const table) {
	return clean_table(table);
}

static table_err dialog_export(Table * const table) {
	char *filename = read_info();
	if (!filename) {
		free(filename);
		return FILE_ERR;
	}
	table_err err = export_table_to_file(table, filename);
	free(filename);
	return err;
}

static table_err dialog_exit([[__maybe_unused__]]Table * const table) {
	return TABLE_EXIT;
}

const operation operations[] = {
	{dialog_insert, "Insert element"},
	{dialog_delete, "Delete by key"},
	{dialog_find, "Search by key"},
	{dialog_find_release, "Search by key and version"},
	{dialog_print, "Print table"},
	{dialog_import, "Import table from file"},
	{dialog_export, "Export table to file"},
	{dialog_clean, "Clean table"},
	{dialog_exit, "EXIT"}
};

void show_menu() {
	for (size_t i = 0; i < COUNT_OP; i++) {
		printf("[%zu]: %s\n", i + 1, operations[i].msg);
	}
	return;
}

static void choice_msg_from_table_err(const table_err err) {
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
	return;
}

int process_choice(Table *table, size_t choice) {
	if (choice < 1 || choice > COUNT_OP) {
		printf("Invalid choice\n");
		return 0;
	}
	table_err err = TABLE_OK;
	err = operations[choice - 1].func(table);
	choice_msg_from_table_err(err);
	if (err == TABLE_EOF || err == TABLE_EXIT) return 1;
	return 0;
}
