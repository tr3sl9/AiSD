#ifndef INFO_STRUCT_H
#define INFO_STRUCT_H

typedef struct {
	size_t info;
} Info;

Info *info_create(void);
int info_insert(Info * const, const size_t number);
void info_print(const Info * const);
void info_free(Info * const);

#endif
