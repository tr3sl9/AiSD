#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "table.h"
#define COUNT_OP sizeof menu_items / sizeof *menu_items

const char *menu_items[] = {
	"Insert element",
	"Delete by key",
	"Search by key",
	"Search by key and version",
	"Print table",
	"Import from file",
	"Export to file",
	"Clean table",
	"Exit"
};

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
	printf("Enter info: ");
	char *info = NULL;
	if (getline(&info, &len, stdin) == -1) {
		printf("Error: Value\n");
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
	return;
}

void dialog_delete(Table * const table) {
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
	table_err err = delete_element(table, key);
	if (err == TABLE_NULL) {
		printf("Error: Table is null\n");
	}
	else if (err == TABLE_VAL) {
		printf("Error: Value\n");
	}
	free(key);
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
	table_err err = search_by_key(table, key);
	if (err == TABLE_NULL) {
		printf("Error: Table is null\n");
	}
	else if (err == TABLE_VAL) {
		printf("Error: Value\n");
	}
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
	printf("Enter version: ");
	RelType version;
	scanf("%zu", &version);
	table_err err = search_by_key_with_release(table, key, version);
	if (err == TABLE_NULL) {
		printf("Error: Table is null\n");
	}
	else if (err == TABLE_VAL) {
		printf("Error: Value\n");
	}
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
	return;
}

void dialog_clean(Table * const table) {
	table_err err = clean_table(table);
	if (err == TABLE_NULL) {
		printf("Error: table is null\n");
		return;
	}
	printf("Table cleaned successfully\n");
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
	table_err err = export_table_to_file(table, filename);
	if (err == FILE_ERR) {
		printf("Error: oppenin file\n");
		return;
	}
	return;	
}

void show_menu() {
	for (size_t i = 0; i < COUNT_OP; i++) {
		printf("%zu. %s\n", i + 1, menu_items[i]);
	}
}

void dialog_exit(Table * const table) {
	exit_from_prog(table);
	return;
}

const functions operation[] = {
	dialog_insert,
	dialog_delete, 
	dialog_find,
	dialog_find_release,
	dialog_print,
	dialog_import,
	dialog_clean,
	dialog_export,
	dialog_exit
};

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
}

int main(){
	Table *table = NULL;
	int choice = 0;
	size_t table_size;
	while (1) {
		printf("Enter table size: ");
		if (scanf("%zu", &table_size) == 1 && table_size > 0) {
			while (getchar() != '\n');
			break;
		}
		printf("Invalide input. Please enter a positive integer\n");
		while (getchar() != '\n');
	}
	table = init_table(table_size);
	if (!table) {
		printf("Failed to inizialize table\n");
		return 1;
	}
	while (1) {
		show_menu();
		while (1) {
			printf("Choice: ");
			if (scanf("%d", &choice) == 1) {
				while (getchar() != '\n');
				if (choice >= 1 && choice <= COUNT_OP + 1) {
					break;
				}
			} else {
				while (getchar() != '\n');
			}
			printf("Invalid choice. Please enter a number between 1 and %d\n", COUNT_OP + 1);
		}
		process_choice(table, choice);
	}
	return 0;
}
