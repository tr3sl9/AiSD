#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "testing.h"
#include "matrix.h"
#include "row.h"
#include "matrix.h"

int main(){
    Matrix *matrix = input_matrix();
    if(matrix == NULL){
        printf("Ошибка при создании матрицы.\n");
        return 1;
    }
    fill_row_for_matrix(&matrix);
    free_matrix(matrix);
    return 0;
}
