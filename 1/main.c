#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "testing.h"
#include "matrix.h"
#include "row.h"
#include "individual.h"

int main(){
	char *prompt_r = "Результат:";
	char *prompt_m = "Матрица:";
    Matrix *matrix = input_matrix();
    if(matrix == NULL){
        printf("Ошибка при создании матрицы\n");
        return 1;
    }
	Row *res_array = calculating_average(matrix);
	if(res_array == NULL){
		printf("Ошибка при создании результирующей строки\n");
		free_matrix(matrix);
		return 1;
	}
	print_matrix(matrix, prompt_m);
	print_row(res_array, prompt_r);
    free_matrix(matrix);
	free_row(res_array);
    return 0;
}
