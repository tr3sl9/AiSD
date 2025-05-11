#include <stdio.h>
#include "testing.h"
#include "table.h"

table_err read_number(size_t * const count, const size_t min, const size_t max){
	int p2 = 0;
	while(p2 != 1){
		printf("Choice: ");
		p2 = scanf("%zu", count);
		if(p2 == 0){
			printf("Error: invalid data type\n");
			printf("Enter the number again: ");
			scanf("%*[^\n]");
		}
		else if(p2 == EOF){
			return TABLE_EOF;
		} else {
			if(*count < min || *count > max){
				p2 = 0;
				printf("The entered number must be greater than %zu and less than or equal to %zu\n", min, max);
				printf("Enter the number again\n");
				scanf("%*[^\n]");
			}
		}
	}
	return 0;
}
