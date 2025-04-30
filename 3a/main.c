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
	int err = 1;
	while (err != EOF) {
        show_menu();
        printf("Choice: ");
		char new_line = ' ';
        int choice;
        err = scanf("%d", &choice);
        if (err == EOF) {
			free_table(table);
            return 1;
        }
        else if (err != 1) {
			printf("Invalid input. Please enter a number.\n");\
			while (new_line != '\n') {
				scanf("%c", &new_line);
			}
            continue;
        }
		while (new_line != '\n') {
				scanf("%c", &new_line);
		}
		if (choice < 1 || choice > COUNT_OP) {
            printf("Invalide choice. Please enter number between 1 and %d\n", COUNT_OP);
            continue;
        }
        if (process_choice(table, choice) == 1) {
            break;
        }
    }
	free_table(table);
	return 1;
}
