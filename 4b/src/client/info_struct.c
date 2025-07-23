#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "info_struct.h"
#include "../lib/sgt_lib.h"

Info *info_create(char * const str) {
    if (!str) {
        return 0;
    }

    Info *info = (Info*)calloc(1, sizeof(Info));
    if (!info) {
        return NULL;
    }

    info->info = strdup(str);
    return info;
} 

void info_print(const Info * const info) {
    if (!info) {
        return;
    }

    printf("%s", info->info);
    return;
}

void info_free(Info * const info) {
    if (!info || !info->info) {
        return;
    }

    free(info->info);
    free(info);
    return;
}

char info_read(Info * const info, FILE * const file) {
    if (!info) {
        return 0;
    }
    
    info->info = read_row_from_file(file);
    if (!info->info) {
        return 0;
    }

    return 1;
}

void info_print_file(const Info * const info, FILE * const file) {
    if (!info) {
        return;
    }

    fprintf(file, "%s", info->info);
    return;
}
