#ifndef QUEUE_LIB_H
#define QUEUE_LIB_H

#include <stdio.h>
#include <stdlib.h>

#include "../lib/bst_lib.h"

typedef struct QueueNode {
    TreeNode *tree_node;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue {
    QueueNode *front;
    QueueNode *back;
} Queue;

typedef enum {
    QUEUE_OK = 0,
    QUEUE_NULL = 1,
    QUEUE_FULL = 2,
    QUEUE_EMPTY = 3,
    QUEUE_MEM = 4
} queue_err;

Queue* queue_create(void);
queue_err queue_push(Queue * const, TreeNode * const);
TreeNode *queue_pop(Queue * const);
TreeNode *queue_peak(const Queue * const);

void queue_free(Queue * const);

char queue_not_empty(const Queue * const);

#endif 
