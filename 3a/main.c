#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "table.h"
#include "dialog.h"

int main(){
	Table *table = create_table();
	size_t choice;
	while (1) {
		show_menu();
		while (1) {
			printf("Choice: ");
			if (scanf("%ld", &choice) == 1) {
				while (getchar() != '\n');
				if (!table_initialized(table) && ((choice >= 2 && choice <= 6) || choice == 8 || choice == 9)) {
						printf("Initialize the table first\n");
					}
				else if (choice >= 1 && choice <= COUNT_OP + 1) {
					break;
				}
			} else {
				while (getchar() != '\n');
			}
			printf("Invalid choice. Please enter a number between 1 and %d\n", COUNT_OP);
		}
		process_choice(table, choice);
	}
	return 0;
}
