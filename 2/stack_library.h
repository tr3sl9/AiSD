#ifndef STACK_LIBRARY_H
#define STACK_LIBRARY_H

typedef struct Stack Stack;

enum state {
	OK = 1;
	NFL = 0;
	NOK = 2;
};

Stack *stack_create(size_t);
int stack_push(Stack*, char*);
int stack_is_full(const Stack*);
int stack_is_empty(const Stack*);
char *stack_pop(Stack*);
void stack_free(Stack*);
#endif
