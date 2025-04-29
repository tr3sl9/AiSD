#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "table.h"
#include "dialog.h"

int main(int argc, char **argv){
	if (argc != 2) {
		printf("Error: not enough argumetns\n");
		return 1;
	}
	if (atoi(argv[1]) <= 0) {
		printf("Error: table size mismatch\n");
		return 1;
	}
	Table *table = create_table(atoi(argv[1]));
	size_t choice;
	int eof_err = 0;
	while (eof_err != EOF) {
		char new_line = ' ';
		show_menu();
		printf("Choice: ");
		if (scanf("%zu", &choice) == EOF) {
			free_table(table);
			eof_err = EOF;
			return 1;
		}
		while (new_line != '\n') {
			scanf("%c", &new_line);
		} 
		if (choice >= 1 && choice <= COUNT_OP + 1) {
			if (process_choice(table, choice) == 1) {
				free_table(table);
				return 0;
			}
		} else {
			printf("Invalid choice. Please enter a number between 1 and %d\n", COUNT_OP);
		}
	}
	free_table(table);
	return 1;
}
