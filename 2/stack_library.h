#ifndef STACK_LIBRARY_H
#define STACK_LIBRARY_H

typedef struct Stack Stack;

typedef enum state {
	OK = 1,
	NOK = 0
} state;

Stack *stack_create(size_t);
state stack_push(Stack*, char*);
state stack_is_full(const Stack*);
state stack_is_empty(const Stack*);
char *stack_pop(Stack*);
void stack_free(Stack*);
char *stack_peek(const Stack*);
#endif
