#ifndef STACK_LIB_H
#define STACK_LIB_H

#include <stdio.h>

#include "../lib/sgt_lib.h"

typedef enum {
    STACK_OK = 0,
    STACK_FULL = 1, 
    STACK_NULL = 2,
    STACK_EMPTY = 3, 
    STACK_MEM = 4,
    STACK_VAL = 5
} stack_err;


typedef struct StackNode {
    TreeNode *tree_node;
    struct StackNode *next;
} StackNode;

typedef struct {
    StackNode *top;
} Stack;

Stack *stack_create(void);
stack_err stack_push(Stack * const, TreeNode * const);
TreeNode *stack_pop(Stack * const);

void stack_free(Stack * const);

char stack_not_empty(const Stack * const);

#endif
