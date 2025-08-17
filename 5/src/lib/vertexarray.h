#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include "../lib/libgraph.h"

typedef struct VertexArray {
    struct Vertex **items;
    size_t count;
    size_t capacity;
} VertexArray;

VertexArray* va_create(void);

void va_free(VertexArray* arr);

char va_push(VertexArray * const arr, struct Vertex * const vertex);
char va_find(const VertexArray * const arr, const struct Vertex * const vertex);

#endif
