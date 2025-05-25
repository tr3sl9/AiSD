#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/bst_lib.h"

Stack *stack_create(void){
    return (Stack*)calloc(1, sizeof(Stack));    
}

stack_err stack_push(Stack * const stack, const TreeNode * const tree_node) {
    if (!stack) {
        return STACK_NULL;
    }

    if (!node) {
        return STACK_VAL;
    }

    StackNode *new_node = (StackNode*)calloc(1, sizeof(StackNode));
    if (!new_node) {
        return STACK_MEM;
    }
    
    new_node->tree_node = tree_node;
    new_node->next = stack->top;
    stack->top = new_node;
    stack->size++;

    return STACK_OK;
}

StackNode *stack_pop(Stack * const stack) {
    if (!stack || !stack->top) {
        return STACK_NULL;
    }

    if (stack->size == 0) {
        return STACK_EMPTY;
    }

    StackNode *current = stack->top;
    stack->top = current->next;
    TreeNode *result = current->tree_node;
    free(current);
    stack_size--;

    return result;
}

void stack_free(Stack * const stack) {    
    while (stack->top) {
        StackNode* temp = stack->top;
        stack->top = stack->top->next;
        free(temp);
    }
    
    free(stack);
    return STACK_OK;
}

char stack_not_empty(const Stack * const stack) {
    return stack->size != 0;
}
