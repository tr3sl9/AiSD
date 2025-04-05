#include <stdio.h>
#include <stdlib.h>
#include "stack_library.h"
#include "game_of_life.h"

typedef struct Stack {
	Generation **container;
	size_t top;
	size_t capacity;
	size_t current_len;
} Stack;

state stack_pop(Stack *stack, Generation **current) {
	if (stack_is_empty(stack) == EMPTY) return EMPTY;
	stack->current_len--;
	stack->top = (stack->top - 1 + stack->capacity) % stack->capacity;
	*current = stack->container[stack->top];
	return OK;
}

state stack_is_empty(const Stack *stack) {
	if ((ssize_t)stack->current_len == -1) return EMPTY;
	return OK;
}

state stack_is_full(const Stack *stack) {
	if (stack == NULL) return EMPTY;
	if (stack->current_len == stack->capacity - 1) return FULL;
	return OK;
}

state stack_push(Stack *stack, Generation *el) {
	if (stack_is_full(stack) == FULL) {
		free_gen(stack->container[stack->top]);
	} else {
		stack->current_len++;
	}
	stack->container[stack->top] = el;
	stack->top = (stack->top + 1) % stack->capacity;
    return OK;
}

Stack *stack_create(const size_t capacity) {
	Stack *stack = (Stack*)malloc(sizeof(Stack));
	if (stack == NULL) return NULL;
	stack->container = (Generation**)calloc(capacity, sizeof(Generation*));
	if (stack->container == NULL) {
		free(stack);
		return NULL;
	}
	stack->top = 0;
	stack->current_len = 0;
	stack->capacity = capacity;
	return stack;
}

void stack_free(Stack *stack){
	if (stack != NULL) {
		while (stack_is_empty(stack) != EMPTY) {
			Generation *current;
			stack_pop(stack, &current);
			free_gen(current);
		}
		free(stack->container);
		free(stack);
	}
	return;
}

Generation *stack_peek(const Stack *stack){
	if(stack_is_empty(stack) == EMPTY) return NULL;
	Generation *copy = copy_gen(stack->container[(stack->top - 1 + stack->capacity) % stack->capacity]);
	return copy;
}
