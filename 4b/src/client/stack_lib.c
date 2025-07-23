#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/sgt_lib.h"
#include "stack_lib.h"

Stack *stack_create(void){
    return (Stack*)calloc(1, sizeof(Stack));    
}

stack_err stack_push(Stack * const stack, TreeNode * const tree_node) {
    if (!stack) {
        return STACK_NULL;
    }

    if (!tree_node) {
        return STACK_VAL;
    }

    StackNode *new_node = (StackNode*)calloc(1, sizeof(StackNode));
    if (!new_node) {
        return STACK_MEM;
    }
    
    new_node->tree_node = tree_node;
    new_node->next = stack->top;
    stack->top = new_node;

    return STACK_OK;
}

TreeNode *stack_pop(Stack * const stack) {
    if (!stack || !stack->top) {
        return NULL;
    }

    StackNode *current = stack->top;
    stack->top = current->next;
    TreeNode *result = current->tree_node;
    free(current);

    return result;
}

void stack_free(Stack * const stack) {    
    while (stack->top) {
        StackNode* temp = stack->top;
        stack->top = stack->top->next;
        free(temp);
    }
    
    free(stack);
    return;
}

char stack_not_empty(const Stack * const stack) {
    return stack->top != NULL;
}

