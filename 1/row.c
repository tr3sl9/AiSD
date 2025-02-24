#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <stdint.h>
#include "row.h"
#include "individual.h"
#include "testing.h"

Err print_res_array(const Row *res_array){
    if(res_array == NULL){
        return ERR_NULL;
    }
    printf("Результат: ");
    for(size_t i = 0; i < res_array->len; i++){
        printf("%f ", res_array->numbers[i]);
    }
    printf("\n");
    return ERR_OK;
}

Row *create_row(const size_t len){
    Row *res_row = (Row*)calloc(1, sizeof(Row));
	if(res_row == NULL){
		return NULL;
	}
    res_row->len = len;
    res_row->numbers = (float*)calloc(len, sizeof(float));
    return res_row;
}

void free_row(Row *res_array){
    if(res_array != NULL){
        if(res_array->numbers != NULL){
            free(res_array->numbers);
        }
        free(res_array);
    }
}

Err input_row(Row *row){
	for(size_t j = 0; j < row->len; j++){
		printf("%zu = ", j + 1);
		if(check_error_for_float(&(row->numbers[j]), -FLT_MAX, FLT_MAX) == END_PROGRAM){
			return END_PROGRAM;
		}
	}
	return ERR_OK;
}
