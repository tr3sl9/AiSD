#include <stdio.h>
#include "testing.h"

Err check_error_for_st(size_t *count, size_t min, size_t max){
	int p2 = 0;
	while(p2 != 1){
		p2 = scanf("%zu", count);
		if(p2 == 0){
			printf("Харам, неверный тип данных \n");
			printf("Введите число еще раз: ");
			scanf("%*[^\n]");
		}
		else if(p2 == EOF){
			return END_PROGRAM;
		} else {
			if(*count <= min || *count >= max){
				p2 = 0;
				printf("Харам, введенное число должно быть больше %zu и меньше %zu\n", min, max);
				printf("Введите число еще раз: ");
				scanf("%*[^\n]");
			}
		}
	}
	return 0;
}

Err check_error_for_float(float *count, float min, float max){
	int p2 = 0;
	while(p2 != 1){
		p2 = scanf("%f", count);
		if(p2 == 0){
			printf("Харам, неверный тип данных \n");
			printf("Введите число еще раз: ");
			scanf("%*[^\n]");
		}
		else if(p2 == EOF){
			return END_PROGRAM;
		} else {
			if(*count <= min || *count >= max){
				p2 = 0;
				printf("Харам, введенное число должно быть больше %f и меньше %f\n", min, max);
				printf("Введите число еще раз: ");
				scanf("%*[^\n]");
			}
		}
	}	
	return 0;
}
