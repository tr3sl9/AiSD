#ifndef INFO_STRUCT_H
#define INFO_STRUCT_H

typedef struct {
    size_t info;
} Info;

Info *info_create(const size_t number);

char info_read(Info * const, FILE * const file);

void info_print(const Info * const);
void info_print_file(const Info * const, FILE * const);
void info_free(Info * const);


#endif
