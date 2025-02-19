#ifndef MATRIX_H
#define MATRIX_H
#include "row.h"
#include "testing.h"

typedef struct Matrix {
	size_t rows_count;
	Row *row;
} Matrix;

Err print_matrix(Matrix *);
Err fill_row_for_matrix(Matrix **);
Matrix *create_matrix(const size_t);
void free_matrix(Matrix *);
Matrix *input_matrix();
#endif
