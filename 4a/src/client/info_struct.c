#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "info_struct.h"

Info *info_create(const size_t number) {
    Info *info = (Info*)calloc(1, sizeof(Info));
    if (!info) {
        return NULL;
    }

    info->info = number;
    return info;
} 

void info_print(const Info * const info) {
    if (!info) {
        return;
    }
    printf("%zu", info->info);
    return;
}

void info_free(Info * const info) {
    free(info);
    return;
}

char info_read(Info * const info, FILE * const file) {
    if (!info) {
        return 0;
    }

    if (fscanf(file, "%zu\n", &info->info) != 1) {
        return 0;
    }

    return 1;
}

void info_print_file(const Info * const info, FILE * const file) {
    if (!info) {
        return;
    }

    fprintf(file, "%zu", info->info);
    return;
}
