#ifndef ROW_H
#define ROW_H
#include "testing.h"

typedef struct Row {
	size_t len;
	float *numbers;
} Row;

Err print_res_array(Row *);
Err fill_row_for_res_array(Row *, const float, const float, const size_t);
Row *create_row(const size_t);
void free_row(Row *);
Row *input_row(size_t rows_count);
#endif
