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
			printf("Error: Value\n");
			printf("Try again\n;");
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
			printf("Error: Value\n");
			printf("Try again\n");
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
		if (scanf("%zu", &number) != 1 || number <= 0) {
			printf("Error: Value\n");
			printf("Try again\n");
		} else {
			break;
		}
	}
	return number;
}

void dialog_insert(Table * const table) {
	if (!table || !table_initialized(table)) {
		printf("Initialize the table first\n");
		return;
	}
	if (table->csize >= table->msize) {
		printf("Error: Table is full\n");
		return;
	}
	KeyType key = read_key();
	if (!key) {
		return;
	}
	InfoType info = read_info();
	if (!info) {
		return;
	}
	table_err err = insert_element(table, key, info);
	if (err == TABLE_NULL) {
		printf("Error: Table is null\n");
	}
	else if (err == TABLE_VAL) {
		printf("Error: Value\n");
	}
	free(key);
	free(info);
	printf("\n###Information added successfully in table\n\n");
	return;
}

void dialog_delete(Table * const table) {
	if (!table || !table_initialized(table)) {
		printf("Initialize the table first\n");
		return;
	}
	KeyType key = read_key();
	if (!key) {
		return;
	}
	table_err err = delete_element(table, key);
	if (err == TABLE_NULL) {
		printf("Error: Table is null\n");
	}
	else if (err == TABLE_VAL) {
		printf("Error: Value\n");
	}
	free(key);
	printf("\n###Information deleted successfully from table\n\n");
	return;
}

void dialog_print(Table * const table) {
	if (!table || !table_initialized(table)) {
		printf("Initialize the table first\n");
		return;
	}
	if (print_table(table) != TABLE_OK) {
		printf("Error: Table is null\n");
	}
	return;
}

void dialog_find(Table * const table) {
	if (!table || !table_initialized(table)) {
		printf("Initialize the table first\n");
		return;
	}
	KeyType key = read_key();
	if (!key) {
		return;
	}
	Table *result = search_by_key(table, key);
	if (!result || result->csize == 0) {
		printf("Error\n");
		return;
	} else {
		printf("\n###Search results:\n\n");
		print_table(result);
	}
	free_table(result);
	free(key);
	return;
}

void dialog_find_release(Table * const table) {
	if (!table || !table_initialized(table)) {
		printf("Initialize the table first\n");
		return;
	}
	KeyType key = read_key();
	if (!key) {
		return;
	}
	RelType release = read_number();
	if (release == (RelType)(-1)) {
		return;
	}
	Table *result = search_by_key_with_release(table, key, release);
	if (!result || result->csize == 0) {
		printf("Error\n");
		return;
	} else {
		printf("\n###Search results:\n\n");
		print_table(result);
	}
	free_table(result);
	free(key);
	return;
}

void dialog_import(Table * const table) {
	if (!table || !table_initialized(table)) {
		printf("Initialize the table first\n");
		return;
	}
	char *filename = read_info();
	if (!filename) {
		return;
	}
	table_err err = import_table_from_file(table, filename);
	if (err == FILE_ERR) {
		printf("Error: file\n");
		return;
	}
	else if (err == TABLE_MAGIC_WORD) {
		printf("Error: magic_wrod in file\n");
		return;
	}
	else if (err == TABLE_SIZE) {
		printf("Error: table size\n");
		return;
	}
	else if (err == TABLE_FULL) {
		printf("Error: table full\n");
		return;
	}
	printf("\n###Table imported successfully\n\n");
	return;
}

void dialog_clean(Table * const table) {
	if (!table || !table_initialized(table)) {
		printf("Initialize the table first\n");
		return;
	}
	table_err err = clean_table(table);
	if (err == TABLE_NULL) {
		printf("Error: table is null\n");
		return;
	}
	printf("\n###Table cleaned successfully\n\n");
	return;
}

void dialog_export(Table * const table) {
	if (!table || !table_initialized(table)) {
		printf("Initialize the table first\n");
		return;
	}
	char *filename = read_info();
	if (!filename) {
		return;
	}
	table_err err = export_table_to_file(table, filename);
	if (err == FILE_ERR) {
		printf("Error: opening file\n");
		return;
	}
	printf("\n###Table exported successfully\n\n");
	return;
}

void dialog_init_table(Table *table) {
	if (table_initialized(table)) {
		printf("Table has already been initialized\n");
		return;
	}
	IndexType msize = read_number();
	table = init_table(table, msize);
	printf("\n###Table initialized successfully\n\n");
	return;
}

void dialog_exit(Table * const table) {
	exit_from_prog(table);
	return;
}

const functions operation[] = {
	dialog_init_table,
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
	"Init table",
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
		printf("%zu. %s\n", i + 1, menu_items[i]);
	}
	return;
}

void process_choice(Table *table, size_t choice) {
	if (choice < 1 || choice > COUNT_OP) {
		printf("Invalid choice\n");
		return;
	}
	if (choice == COUNT_OP + 1) {
		dialog_exit(table);
	} else {
		operation[choice - 1](table);
	}
	return;
}
