#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "matrix.h"
#include "row.h"

Err print_res_array(Row *res_array){
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

Err fill_row_for_res_array(Row *res_array, const float max, const float average_value, const size_t i){
	if(max == 0){
//это случай деления на 0, без разницы 0/0 или x/0, все под один if, будем выводить в блок просто 0(без этого в блоке будет значение -nan)
		res_array->numbers[i] = 0;
	}
	else{
		res_array->numbers[i] = average_value / max;
	}
    return ERR_OK;
}

Row *create_row(const size_t len){
    Row *res_row = calloc(1, sizeof(Row));
    res_row->len = len;
    res_row->numbers = calloc(len, sizeof(float));
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

Row *input_row(size_t rows_count){
    Row *res_array = create_row(rows_count);
    return res_array;
}
