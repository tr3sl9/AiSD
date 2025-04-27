#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "table.h"
#include "dialog.h"

int main(int argc, char **argv){
	if (argc != 2) {
		printf("Error: not enough argumetns\n");
		return 0;
	}
	Table *table = create_table(atoi(argv[1]));
	size_t choice;
	while (1) {
		show_menu();
		while (1) {
			printf("Choice: ");
			if (scanf("%ld", &choice) == 1) {
				while (getchar() != '\n');
				if (choice >= 1 && choice <= COUNT_OP + 1) {
					break;
				}
			}
			printf("Invalid choice. Please enter a number between 1 and %d\n", COUNT_OP);
		}
		if (process_choice(table, choice) == 1) break;
	}
	return 0;
}
