#ifndef TESTING_H
#define TESTING_H

typedef enum Err {
	ERR_NULL = 1,
	END_PROGRAM = 2,
	ERR_OK = 0,
} Err;

Err check_error_for_st(size_t *, size_t, size_t);
Err check_error_for_float(float *, float, float); 
#endif
