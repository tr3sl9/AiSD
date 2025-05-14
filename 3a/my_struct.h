#ifndef MY_STRUCT_H
#define MY_STRUCT_H

typedef struct {
    void * array;
    size_t capacity;
    size_t count; 
    size_t size_of_one;
} DynamicArray;

DynamicArray *da_create(const size_t size_of_one);
int da_append(DynamicArray*, const void *item);
void da_print(const DynamicArray * const, void(*print_item)(const void *));
void da_free(DynamicArray * const);
int da_delete_el(DynamicArray *da, size_t index);

#endif
