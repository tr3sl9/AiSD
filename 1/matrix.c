#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <stdlib.h>
#include "testing.h"
#include "matrix.h"
#include "row.h"

Err print_matrix(const Matrix *matrix){
    if(matrix == NULL){
		return ERR_NULL;
	}
	printf("Матрица: \n");
    for(size_t i = 0; i < matrix->rows_count; i++) {
        for(size_t j = 0; j < matrix->row[i].len; j++){
            printf("%f ", matrix->row[i].numbers[j]);
        }
        printf("\n");
    }
    return ERR_OK;
}

Matrix *fill_row_for_matrix(Matrix *matrix){
	if(matrix == NULL){
		return NULL;
	}
    for(size_t i = 0; i < matrix->rows_count; i++){
        printf("Введите количество элементов строки %zu > ", i + 1);
        size_t len = 0;
        if(check_error_for_st(&len, 0, INT_MAX) == END_PROGRAM){
			return NULL;
		}
        matrix->row[i].numbers = (float*)realloc(matrix->row[i].numbers, len * sizeof(float));
		matrix->row[i].len = len;
        if(matrix->row[i].numbers == NULL){
            return NULL;
        }
        for(size_t j = 0; j < len; j++){
            printf("%zu = ", j + 1);
            if(check_error_for_float(&(matrix->row[i].numbers[j]), -FLT_MAX, FLT_MAX) == END_PROGRAM){
                return NULL;
            }
	    }
    }
    return matrix;
}


Matrix *create_matrix(const size_t len){
    Matrix *matrix = (Matrix*)calloc(1, sizeof(Matrix));
    if(matrix == NULL){
        return NULL;
    }
    matrix->rows_count = len;
    matrix->row = (Row*)calloc(len, sizeof(Row));
    if(matrix->row == NULL){
        free(matrix);
        return NULL;
    }
    return matrix;
}

void free_matrix(Matrix *matrix){
    if(matrix != NULL){
        if(matrix->row != NULL){
            for(size_t i = 0; i < matrix->rows_count; i++){
                if(matrix->row[i].numbers != NULL){
                    free(matrix->row[i].numbers);
                }
            }
            free(matrix->row);
        }
        free(matrix);
    }
}

Matrix *input_matrix(){
    size_t len = 0;
    printf("Введите количество строк матрицы > ");
    if(check_error_for_st(&len, 0, INT_MAX) == END_PROGRAM) return NULL;
    Matrix *matrix = create_matrix(len);
    Matrix *changed_matrix = fill_row_for_matrix(matrix);
	if(changed_matrix == NULL){
		printf("Ошибка при заполнении матрицы\n");
		free_matrix(matrix);
		return NULL;
	}
    return matrix;
}
