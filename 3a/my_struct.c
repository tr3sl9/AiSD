#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "table.h"
#include "my_struct.h"

DynamicArray *da_create(const size_t size_of_one) {
	DynamicArray *da = malloc(sizeof(DynamicArray));
	if (!da) return NULL;

	da->capacity = 4;
	da->count = 0;
	da->size_of_one = size_of_one;
	da->array = calloc(da->capacity, size_of_one);

	if (!da->array) {
		free(da);
		return NULL;
	}

	return da;
}

int da_append(DynamicArray *da, const void *item) {
	if (da->count == da->capacity) {
		size_t new_capacity = da->capacity * 2;
		void *new_array = realloc(da->array, new_capacity * da->size_of_one);
		if (!new_array) return 0;
		da->array = new_array;
		da->capacity = new_capacity;
	}
	memcpy((char*)da->array + da->count * da->size_of_one, item, da->size_of_one);
	da->count++;
	return 1; 
}

void da_print(const DynamicArray * const da, void(*print_item)(const void *)) {
	printf("Dynamic Array (count: %zu, capacity: %zu):\n", da->count, da->capacity);
	for (size_t i = 0; i < da->count; i++) {
		printf("[%zu] ", i);
        print_item((char *)da->array + i * da->size_of_one);
        printf("\n");
    }
}

void da_free(DynamicArray * const da) {
    if (da) {
        free(da->array);
        free(da);
    }
}
