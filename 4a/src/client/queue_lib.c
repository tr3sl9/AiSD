#include <stdio.h>
#include <stdlib.h>
#include <string.h>

queue_err queue_push(Queue * const queue, TreeNode * const node) {
    if (!queue || !queue->data) {
        return QUEUE_NULL;
    }
    if (queue->capacity == queue->size) {
        return QUEUE_FULL;
    }

    queue->data[queue->back] = node;
    queue->back++;
    queue->size++;

    return QUEUE_OK;
}

Queue *queue_create(const size_t capacity) {
    if (!capacity) {
        return NULL;
    }

    Queue *queue = (Queue*)calloc(1, sizeof(Queue));
    if (!queue) {
        return NULL;
    }

    queue->data = (TreeNode**)calloc(capacity, sizeof(TreeNode*));
    if (!queue->data) {
        free(queue);
        return NULL;
    }

    queue->capacity = capacity;
    return queue;
}

TreeNode *queue_pop(Queue * const queue) {
    if (!queue || !queue->data) {
        return NULL;
    }

    TreeNode *node = queue->data[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;

    return node;
}

TreeNode *queue_peak(const Queue * const queue) {
    if (!queue || !queue->data) {
        return NULL;
    }

    return queue->data[queue->front];
}

void queue_free(Queue * const queue) {
    free(queue->data);
    free(queue);
    return;
}

char queue_not_empty(const Queue * const queue) {
    return queue->size != 0;
}
