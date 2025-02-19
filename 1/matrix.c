#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <stdlib.h>
#include "testing.h"
#include "matrix.h"
#include "row.h"

Err print_matrix(Matrix *matrix){
    if(matrix == NULL) {
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

Err fill_row_for_matrix(Matrix **matrix){
    Row *res_array = input_row((*matrix)->rows_count);
    if(res_array == NULL){
        return ERR_NULL;
    }
    for(size_t i = 0; i < (*matrix)->rows_count; i++){
        printf("Введите количество элементов строки %zu > ", i + 1);
        size_t len = 0;
        if(check_error_for_st(&len, 0, INT_MAX) == END_PROGRAM){
			free_row(res_array);
			return END_PROGRAM;
		}
        (*matrix)->row[i].numbers = (float*)realloc(((*matrix)->row)[i].numbers, len * sizeof(float));
		(*matrix)->row[i].len = len;
        if((*matrix)->row[i].numbers == NULL){
            printf("Ошибка выделения памяти\n");
			free_row(res_array);
            return ERR_NULL;
        }
        float average_value = 0;
        float max = -1e10;
        for(size_t j = 0; j < len; j++){
            printf("%zu = ", j + 1);
            if(check_error_for_float(&((*matrix)->row[i].numbers[j]), -FLT_MAX, FLT_MAX) == END_PROGRAM){
                return END_PROGRAM;
            }
            average_value += (*matrix)->row[i].numbers[j];
            if((*matrix)->row[i].numbers[j] > max){
                max = (*matrix)->row[i].numbers[j];
            }
        }
        average_value /= (float)len;
        fill_row_for_res_array(res_array, max, average_value, i);
    }
    print_matrix(*matrix);
    print_res_array(res_array);
    free_row(res_array);
    return ERR_OK;
}

Matrix *create_matrix(const size_t len){
    Matrix *matrix = calloc(1, sizeof(Matrix));
    if(matrix == NULL){
        return NULL;
    }
    matrix->rows_count = len;
    matrix->row = calloc(len, sizeof(Row));
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
    return matrix;
}
