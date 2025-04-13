#include <stdio.h>
#include <stdlib.h>
#include "table.h"
#define COUNT_OP 7

const functions operation[COUNT_OP] = {
	dialog_insert,
	dialog_delete, 
	dialog_find,
	dialod_fing_version,
	dialog_print,
	dialog_import,
	dialog_clear
};

void dialog_insert(Table *table) {
	size_t len = 100;
	char *key = NULL;
	printf("Enter key: ");
	if (getline(&key, &len, stdin) == -1) {
		printf("Error\n");
		return;
	}
	char *info = NULL;
	printf("Enter info: ");
	if (getline(&info, &len, stdin) == -1) {
		printf("Error\n");
		return;
	}
	
}

int main(){
	printf();

}
