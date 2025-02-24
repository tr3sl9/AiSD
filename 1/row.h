#ifndef ROW_H
#define ROW_H
#include "testing.h"

typedef struct Row {
	size_t len;
	float *numbers;
} Row;

Err print_res_array(const Row *);
Row *create_row(const size_t);
void free_row(Row *);
#endif
