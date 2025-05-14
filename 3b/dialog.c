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
#define PROMPT_FOR_FILE "Enter filename: "
#define PROMPT_FOR_NUMBER "Enter number: "
#define COUNT_OP sizeof(operations) / sizeof(operation)

char* read_info(const char * const prompt) {
	char* info = NULL;
	while (!info) {
		info = readline(prompt);
		if (info == NULL) {
			return NULL;
		} 
	}
	return info;
}

table_err dialog_insert(Table * const table) {
	if (table->csize >= table->msize) {
		return TABLE_FULL;
	}

	char *key = read_info(PROMPT_FOR_KEY);
	if (!key) {
		free(key);
		return TABLE_EOF;
	}

	size_t info = 0;
	if (read_number(&info, 0, SIZE_MAX, PROMPT_FOR_NUMBER) == TABLE_EOF) {
		free(key);
		return TABLE_EOF;
	}

	table_err err = insert_key_to_table(table, key, (size_t)info);

	free(key);
	return err;
}

table_err dialog_delete(Table * const table) {
    if (table->csize == 0) {
		return TABLE_EMPTY;
	}
    
    char* key = read_info(PROMPT_FOR_KEY);
    if (!key) {
		free(key);
		return TABLE_EOF;
	}

    table_err err = delete_key_from_table(table, key);

    free(key);
    return err;
}

table_err dialog_find(Table * const table) {
    if (table->csize == 0) {
		return TABLE_EMPTY;
	}
    
    char* key = read_info(PROMPT_FOR_KEY);
    if (!key) {
		free(key);
		return TABLE_EOF;
	}

	size_t count_key = 0; 
    KeySpace** elements = search_by_key_in_table(table, key, &count_key);
    if (!elements) {
        free(key);
        return TABLE_VAL;
    }
    
    printf("\nFound %zu elements with key '%s':\n", count_key, key);
    printf("┌───────────┬──────────────────────┐\n");
    printf("│ Release   │ Info                 │\n");
    printf("├───────────┼──────────────────────┤\n");
    for (size_t i = 0; i < count_key; i++) {
        printf("│ %-9zu │ %-20zu │\n", elements[i]->release, elements[i]->info->info);
        if (i < count_key - 1) {
            printf("├───────────┼──────────────────────┤\n");
        }
		info_free(elements[i]->info);
		free_ks(elements[i]);
		free(elements[i]);
    }
    printf("└───────────┴──────────────────────┘\n");
    
    free(elements);
    free(key);
    return TABLE_OK;
}

table_err dialog_find_release(Table * const table) {
    if (table->csize == 0) {
		return TABLE_EMPTY;
	}
    
    char* key = read_info(PROMPT_FOR_KEY);
    if (!key) {
		free(key);
		return TABLE_EOF;
	}
    
	size_t release = 0;
    if (read_number(&release, 0, SIZE_MAX, PROMPT_FOR_NUMBER) == TABLE_EOF) {
        free(key);
        return TABLE_EOF;
    }
    
    KeySpace* element = search_by_key_with_release_in_table(table, key, release);
    if (!element) {
        free(key);
        return TABLE_VAL;
    }
    
    printf("\nFound element with key '%s' and release %zu:\n", key, release);
    printf("┌──────────────────────┐\n");
    printf("│ Info                 │\n");
    printf("├──────────────────────┤\n");
    printf("│ %-20zu │\n", element->info->info);
    printf("└──────────────────────┘\n");
    
	info_free(element->info);
    free_ks(element);
	free(element);
	free(key);
    return TABLE_OK;
}

table_err dialog_print(Table * const table) {
    return print_table(table);
}

table_err dialog_import(Table * const table) {
    char* filename = read_info(PROMPT_FOR_FILE);
    if (!filename) {
		free(filename);
		return FILE_ERR;
	}
    
    table_err err = import_table_from_file(table, filename);
    free(filename);
    return err;
}

table_err dialog_export(Table * const table) {
    char* filename = read_info(PROMPT_FOR_FILE);
    if (!filename) {
		free(filename);
		return TABLE_EOF;
	}
    
    table_err err = export_table_to_file(table, filename);
    free(filename);
    return err;
}

static table_err dialog_exit(__attribute__((__unused__))Table * const table) {
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
	{dialog_exit, "EXIT"}
};

void show_menu() {
    printf("\nMenu:\n");
    for (size_t i = 0; i < COUNT_OP; i++) {
        printf("[%zu]: %s\n", i + 1, operations[i].msg);
    }
    printf("\n");
	return;
}

static void choice_msg_from_table_err(const table_err err) {
    switch (err) {
        case TABLE_OK: printf("Operation completed successfully\n"); break;
        case TABLE_EMPTY: printf("Error: Table is empty\n"); break;
        case TABLE_FULL: printf("Error: Table is full\n"); break;
        case TABLE_MEM: printf("Error: Memory allocation failed\n"); break;
        case TABLE_NULL: printf("Error: Table is null\n"); break;
        case TABLE_VAL: printf("Error: Invalid value\n"); break;
        case TABLE_SIZE: printf("Error: Table size mismatch\n"); break;
        case TABLE_MAGIC_WORD: printf("Error: Invalid file format\n"); break;
        case FILE_ERR: printf("Error: Cannot open file\n"); break;
        case TABLE_EOF: printf("Error: EOF\n"); break;
        case TABLE_EXIT: printf("EXIT\n"); break;
        default: printf("Unknown error\n");
    }
}

int process_choice(Table *table, size_t choice) {
	if (choice < 1 || choice > COUNT_OP) {
		printf("Invalid choice\n");
		return 0;
	}

	table_err err = operations[choice - 1].func(table);
	choice_msg_from_table_err(err);

	if (err == TABLE_EOF || err == TABLE_EXIT) return 1;
	return 0;
}
