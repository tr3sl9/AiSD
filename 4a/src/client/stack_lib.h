#ifndef STACK_LIB_H
#define STACK_LIB_H

#include <stdio.h>

#include "../lib/bst_lib.h"

typedef enum {
    STACK_OK = 0,
    STACK_FULL = 1, 
    STACK_NULL = 2,
    STACK_EMPTY = 3, 
    STACK_MEM = 4
} stack_err;

typedef struct {
    TreeNode *tree_node;
    struct StackNode *next;
} StackNode;

typedef struct {
    StackNode *top;
    size_t size;
} Stack;

Stack *stack_create(void);
stack_err stack_push(Stack * const, const TreeNode * const);
StackNode *stack_pop(Stack * const);

void stack_free(Stack * const);

char stack_not_empty(const Stack * const);

#endif
