#ifndef TESTING_H
#define TESTING_H

#include "../lib/table.h"

table_err read_positive_number(size_t * const variable_for_number, const char * const prompt);
table_err read_number(size_t * const variable_for_number, const size_t lower_bound_number, const size_t upper_bound_number, const char * const prompt);

#endif
