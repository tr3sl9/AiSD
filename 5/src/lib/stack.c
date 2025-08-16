#include <stdlib.h>
#include "stack.h"

Stack* stack_create(void) {
    return (Stack*)calloc(1, sizeof(Stack));
}

void stack_free(Stack * const stack) {
    if (!stack) {
        return;
    }

    StackNode *current = stack->head;
    while (current) {
        StackNode *next = current->next;
        free(current);
        current = next;
    }

    free(stack);

    return;
}

char stack_push(Stack * const stack, void * const item) {
    if (!stack) {
        return 0;
    }

    StackNode *node = (StackNode*)malloc(sizeof(StackNode));
    if (!node) {
        return 0;
    }

    node->value = item;
    node->next = stack->head;
    stack->head = node;

    return 1;
}

void* stack_pop(Stack * const stack) {
    if (!stack || !stack->head) {
        return NULL;
    }

    StackNode *node = stack->head;
    void *vertex = node->value;
    stack->head = node->next;
    free(node);

    return vertex;
}

void* stack_peek(const Stack * const stack) {
    if (!stack || !stack->head) {
        return NULL;
    }

    return stack->head->value;
}

size_t stack_size(const Stack * const stack) {
    if (!stack) {
        return 0;
    }

    size_t cnt = 0;
    const StackNode* node = stack->head;
    while (node) { 
        cnt++; 
        node = node->next; 
    }

    return cnt;
}

char stack_is_empty(const Stack * const stack) {
    return (!stack || stack->head == NULL) ? 1 : 0;
}

int stack_contains(const Stack * const stack, const void * const item) {
    if (!stack) {
        return 0;
    }

    const StackNode* node = stack->head;
    while (node) {
        if (node->value == item) {
            return 1;
        }

        node = node->next;
    }

    return 0;
}


