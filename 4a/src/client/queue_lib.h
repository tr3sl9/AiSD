#ifndef QUEUE_LIB_H
#define QUEUE_LIB_H

#include <stdio.h>
#include <stdlib.h>

#include "../lib/bst_lib.h"

typedef struct {
    TreeNode** data;
    size_t capacity;  
    size_t size;    
    size_t front;   
    size_t back;
} Queue;

typedef enum {
    QUEUE_OK = 0,
    QUEUE_NULL = 1,
    QUEUE_FULL = 2,
    QUEUE_EMPTY = 3,
    QUEUE_MEM = 4
} queue_err;

Queue* queue_create(const size_t capacity);
queue_err queue_push(Queue * const, TreeNode * const);
TreeNode *queue_pop(Queue * const);
TreeNode *queue_peak(const Queue * const);

char queue_not_empty(const Queue * const);

void queue_free(Queue * const);

#endif 
