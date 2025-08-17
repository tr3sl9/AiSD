#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "priorityqueue.h"

PriorityQueue* pq_create(void) {
    return (PriorityQueue*)calloc(1, sizeof(PriorityQueue));
}

void pq_free(PriorityQueue * const pq) {
    if (!pq) {
        return;
    }

    free(pq->nodes);
    free(pq);

    return;
}

void pq_resize(PriorityQueue * const pq) {
    if (!pq) {
        return;
    }

    size_t new_cap = pq->capacity ? pq->capacity * 2 : 16;
    PQNode* new_nodes = (PQNode*)realloc(pq->nodes, new_cap * sizeof(PQNode));
    if (!new_nodes) {
        return;
    }

    pq->nodes = new_nodes;
    pq->capacity = new_cap;

    return;
}

void pq_bubble_up(PriorityQueue * const pq, size_t i) {
    if (!pq) {
        return;
    }

    while (i > 0) {
        size_t parent = (i - 1) / 2;
        if (pq->nodes[parent].priority <= pq->nodes[i].priority) {
            break;
        }
        
        PQNode temp = pq->nodes[parent];
        pq->nodes[parent] = pq->nodes[i];
        pq->nodes[i] = temp;
        i = parent;
    }
    
    return;
}

char pq_insert(PriorityQueue * const pq, Vertex * const vertex, const size_t priority) {
    if (!pq || !vertex) {
        return 0;
    }
    
    if (pq->size == pq->capacity){
        pq_resize(pq); 
    }

    if (!pq->nodes) {
        return 0;
    }

    pq->nodes[pq->size].vertex = vertex;
    pq->nodes[pq->size].priority = priority;
    pq->size++;
    
    pq_bubble_up(pq, pq->size - 1);

    return 1;
}

void pq_heapify_down(PriorityQueue * const pq) {
    if (!pq || !pq->nodes || pq->size == 0) {
        return;
    }

    size_t i = 0;
    while (1) {
        size_t left = 2 * i + 1;
        size_t right = 2 * i + 2;
        size_t smallest = i;
        
        if (left < pq->size && pq->nodes[left].priority < pq->nodes[smallest].priority) {
            smallest = left;
        }
        if (right < pq->size && pq->nodes[right].priority < pq->nodes[smallest].priority) {
            smallest = right;
        }
        
        if (smallest == i) {
            break;
        }
        
        PQNode temp = pq->nodes[i];
        pq->nodes[i] = pq->nodes[smallest];
        pq->nodes[smallest] = temp;
        i = smallest;
    }

    return;
}

Vertex* pq_extract_min(PriorityQueue * const pq) {
    if (!pq || !pq->nodes || pq->size == 0) {
        return NULL;
    }
    
    Vertex* min_vertex = pq->nodes[0].vertex;
    
    pq->nodes[0] = pq->nodes[pq->size - 1];
    pq->size--;
    
    pq_heapify_down(pq);

    return min_vertex;
}

size_t pq_get_min_priority(PriorityQueue* pq) {
    if (!pq || pq->size == 0) {
        return SIZE_MAX;
    }

    return pq->nodes[0].priority;
}

int pq_is_empty(const PriorityQueue* pq) {
    return !pq || pq->size == 0;
}

int pq_decrease_key(PriorityQueue* pq, Vertex* vertex, size_t new_priority) {
    if (!pq || !vertex || pq->size == 0) {
        return 0;
    }
    
    size_t i;
    for (i = 0; i < pq->size; i++) {
        if (pq->nodes[i].vertex == vertex) {
            break;
        }
    }
    
    if (i >= pq->size) {
        return 0;
    }

    if (new_priority >= pq->nodes[i].priority) {
        return 0;
    }
    
    pq->nodes[i].priority = new_priority;
    
    pq_bubble_up(pq, i);

    return 1;
}
