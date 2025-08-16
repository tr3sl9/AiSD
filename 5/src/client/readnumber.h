#ifndef READ_NUMBER_H
#define READ_NUMBER_H

#include "../lib/libgraph.h"

graph_err read_positive_number(size_t * const variable_for_number, const char * const prompt);
graph_err read_number(int * const variable_for_number, const int lower_bound_number, const int upper_bound_number, const char * const prompt);
graph_err read_number_double(double * const variable_for_number, const double lower_bound_number, const double upper_bound_number, const char * const prompt);

#endif
