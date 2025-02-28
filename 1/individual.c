#include <stdio.h>
#include <float.h>
#include "row.h"
#include "matrix.h"

Row *calculating_average(const Matrix *matrix){
	if(matrix == NULL){
		return NULL;
	}
	Row *res_array = create_row(matrix->rows_count);
	for(size_t i = 0; i < matrix->rows_count; i++){
		for(size_t j = 0; j < matrix->row[i].len; j++){
			res_array->numbers[i] += matrix->row[i].numbers[j];
		}
	}
	float max = -FLT_MAX;
	for(size_t i = 0; i < res_array->len; i++){
		if(res_array->numbers[i] > max) max = res_array->numbers[i];
	}
	for(size_t i = 0; i < res_array->len; i++){
		res_array->numbers[i] /= max;
	}
    return res_array;
}
