#ifndef STACK_H
#define STACK_H

typedef struct StackNode {
    void *value;
    struct StackNode *next;
} StackNode;

typedef struct Stack {
    StackNode *head;
} Stack;

Stack* stack_create(void);

void stack_free(Stack * const stack);
void* stack_pop(Stack * const stack);
void* stack_peek(const Stack * const stack);

char stack_push(Stack * const stack, void * const item);
char stack_is_empty(const Stack * const stack);

size_t stack_size(const Stack * const stack);

int stack_contains(const Stack * const stack, const void * const item);

#endif


