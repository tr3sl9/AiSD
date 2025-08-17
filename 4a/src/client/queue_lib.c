#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib//bst_lib.h"
#include "queue_lib.h"

queue_err queue_push(Queue * const queue, TreeNode * const node) {
    if (!queue || !node) {
        return QUEUE_NULL;
    }

    QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
    if (!new_node) {
        return QUEUE_MEM;
    }

    new_node->tree_node = node;
    new_node->next = NULL;

    if (queue->back) {
        queue->back->next = new_node;
    } else {
        queue->front = new_node;
    }
    queue->back = new_node;

    return QUEUE_OK;
}

Queue *queue_create(void) {
    Queue *queue = (Queue*)calloc(1, sizeof(Queue));
    return queue;
}

TreeNode *queue_pop(Queue * const queue) {
    if (!queue) {
        return NULL;
    }

    QueueNode* front_node = queue->front;
    TreeNode* node = front_node->tree_node;

    queue->front = front_node->next;
    if (!queue->front) {
        queue->back = NULL;
    }

    free(front_node);
    return node;
}

TreeNode *queue_peek(const Queue * const queue) {
    if (!queue || !queue->front) {
        return NULL;
    }

    return queue->front->tree_node;
}

void queue_free(Queue* queue) {
    QueueNode* current = queue->front;
    while (current) {
        QueueNode* next = current->next;
        free(current);
        current = next;
    }

    free(queue);
    return;
}

char queue_not_empty(const Queue * const queue) {
    return queue_peek(queue) != NULL;
}
