#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "info_struct.h"

Info *info_create(void) {
	return (Info*)calloc(1, sizeof(size_t));
} 

//TODO char
int info_insert(Info * const info, const size_t number) {
	if (!info) return 0;
	info->info = number;
	return 1;
}

void info_print(const Info * const info) {
	if (!info) return;
	printf("%zu", info->info);
	return;
}

void info_free(Info * const info) {
	free(info);
	return;
}	
