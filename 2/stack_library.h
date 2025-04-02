#ifndef STACK_LIBRARY_H
#define STACK_LIBRARY_H
#include "game_of_life.h"

typedef struct Stack Stack;

typedef enum state {
	OK = 1,
	NOK = 0
} state;

Stack *stack_create(size_t);
state stack_push(Stack*, Generation*);
state stack_is_full(const Stack*);
state stack_is_empty(const Stack*);
Generation *stack_pop(Stack*);
void stack_free(Stack*);
Generation *stack_peek(const Stack*);
#endif
