#ifndef STACK_LIBRARY_H
#define STACK_LIBRARY_H
#include "game_of_life.h"

typedef struct Stack Stack;

typedef enum state {
	OK = 1,
	FULL = 0,
	EMPTY = -1
} state;

Stack *stack_create(const size_t);
state stack_push(Stack*, Generation*);
state stack_is_full(const Stack*);
state stack_is_empty(const Stack*);
state stack_pop(Stack*, Generation**);
void stack_free(Stack*);
Generation *stack_peek(const Stack*);
#endif
