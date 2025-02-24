#include <stdio.h>
#include <float.h>
#include "row.h"
#include "matrix.h"

Row *calculating_average(const Matrix *matrix){
	if(matrix == NULL){
		return NULL;
	}
	Row *res_array = create_row(matrix->rows_count);
    float max = -FLT_MAX;
	for(size_t i = 0; i < matrix->rows_count; i++){
		float average_value = 0;
		for(size_t j = 0; j < matrix->row[i].len; j++){
			average_value += matrix->row[i].numbers[j];
			if(matrix->row[i].numbers[j] > max){
				max = matrix->row[i].numbers[j];
			}
		}
		average_value /= (float)matrix->rows_count;
		res_array->numbers[i] = average_value / max;
	}
    return res_array;
}
