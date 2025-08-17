#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdint.h>

#include "libgraph.h"

typedef struct PQNode {
    Vertex* vertex;
    size_t priority;
} PQNode;

typedef struct PriorityQueue {
    PQNode* nodes;
    size_t size;
    size_t capacity;
} PriorityQueue;

PriorityQueue* pq_create(void);

void pq_free(PriorityQueue* pq);

Vertex* pq_extract_min(PriorityQueue* pq);

size_t pq_get_min_priority(PriorityQueue* pq);

char pq_insert(PriorityQueue* pq, Vertex* vertex, size_t priority);

int pq_decrease_key(PriorityQueue* pq, Vertex* vertex, size_t new_priority);
int pq_is_empty(const PriorityQueue* pq);

#endif
