#include <stdlib.h>

#include "vertexarray.h"

VertexArray* va_create(void) {
    return (VertexArray*)calloc(1, sizeof(VertexArray));
}

void va_free(VertexArray* arr) {
    if (!arr) {
        return;
    }

    free(arr->items);
    free(arr);

    return;
}

char va_push(VertexArray * const arr, Vertex * const vertex) {
    if (!arr || !vertex) {
        return 0;
    }

    if (arr->count == arr->capacity) {
        size_t new_cap = arr->capacity ? arr->capacity * 2 : 10;
        Vertex** new_items = (Vertex**)realloc(arr->items, new_cap * sizeof(Vertex*));
        if (!new_items) {
            return 0;
        }
        arr->items = new_items;
        arr->capacity = new_cap;
    }

    arr->items[arr->count++] = vertex;

    return 1;
}

char va_find(const VertexArray * const arr, const Vertex * const vertex) {
    if (!arr || !vertex) {
        return 0;
    }

    for (size_t i = 0; i < arr->count; i++) {
        if (arr->items[i] == vertex) {
            return 1;
        }
    }

    return 0;
}
