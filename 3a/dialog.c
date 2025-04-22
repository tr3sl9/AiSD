#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "table.h"
#include "dialog.h"

void dialog_insert(Table * const table) {
	size_t len = 100;
	if (table->csize >= table->msize) {
		printf("Error: Table is full\n");
		return;
	}
	printf("Enter key: ");
	char *key = NULL;
	if (getline(&key, &len, stdin) == -1) {
		printf("Error: Value\n");
		return;
	}
	key[strcspn(key, "\n")] = '\0';
	printf("Enter info: ");
	char *info = NULL;
	if (getline(&info, &len, stdin) == -1) {
		printf("Error: Value\n");
		return;
	}
	info[strcspn(info, "\n")] = '\0';
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
	size_t len = 100;
	printf("Enter key: ");
	char *key = NULL;
	if (getline(&key, &len, stdin) == -1) {
		printf("Error: Value\n");
		return;
	}
	key[strcspn(key, "\n")] = '\0';
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
	if (print_table(table) != TABLE_OK) {
		printf("Error: Table is null\n");
	}
	return;
}

void dialog_find(Table * const table) {
	size_t len = 100;
	printf("Enter key: ");
	char *key = NULL;
	if (getline(&key, &len, stdin) == -1) {
		printf("Error\n");
		return;
	}
	key[strcspn(key, "\n")] = '\0';
	Table *result = search_by_key(table, key);
	if (!result) {
		printf("Error\n");
		return;
	}
	printf("\n###Search results:\n\n");
	print_table(result);
	free_table(result);
	free(key);
	return;
}

void dialog_find_release(Table * const table) {
	size_t len = 100;
	printf("Enter key: ");
	char *key = NULL;
	if (getline(&key, &len, stdin) == -1) {
		printf("Error\n");
		return;
	}
	key[strcspn(key, "\n")] = '\0';
	printf("Enter version: ");
	RelType version;
	scanf("%zu", &version);
	Table *result = search_by_key(table, key);
	if (!result) {
		printf("Error\n");
		return;
	}
	printf("\n###Search results:\n\n");
	print_table(result);
	free_table(result);
	free(key);
	return;
}

void dialog_import(Table * const table) {
	size_t len = 100;
	printf("Enter filename: ");
	char *filename = NULL;
	if (getline(&filename, &len, stdin) == -1) {
		printf("Error\n");
		return;
	}
	filename[strcspn(filename, "\n")] = '\0';
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
	table_err err = clean_table(table);
	if (err == TABLE_NULL) {
		printf("Error: table is null\n");
		return;
	}
	printf("\n###Table cleaned successfully\n\n");
	return;
}

void dialog_export(Table * const table) {
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
	filename[strcspn(filename, "\n")] = '\0';
	table_err err = export_table_to_file(table, filename);
	if (err == FILE_ERR) {
		printf("Error: oppenin file\n");
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
	printf("Enter max size: ");
	IndexType msize;
	while (1) {
		scanf("%zu", &msize);
		if (msize <= 0) {
			printf("The size must be greater than 0\n");
			while (getchar() != '\n');
		} else {
			break;
		}
	}
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
