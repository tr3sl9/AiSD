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

Generation *stack_pop(Stack *stack) {
	if (stack_is_empty(stack)) return NULL;
	stack->current_len--;
	stack->top = (stack->top - 1 + stack->capacity) % stack->capacity;
	return stack->container[stack->top];
}

state stack_is_empty(const Stack *stack) {
	if (stack == NULL) return NOK;
	return stack->current_len == 0;
}

state stack_is_full(const Stack *stack) {
	if (stack == NULL) return NOK;
	return stack->current_len == stack->capacity - 1;
}

state stack_push(Stack *stack, Generation *el) {
	if (stack_is_full(stack)) {
		//              char *old = stack->container[stack->top];
		//              if (old) free(old);
	} else {
		stack->current_len++;
	}
	stack->container[stack->top] = el;
	stack->top = (stack->top + 1) % stack->capacity;
	return OK;
}

Stack *stack_create(size_t capacity) {
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
		while (!stack_is_empty(stack)) {
			Generation *current = stack_pop(stack);
			free_gen(current);
		}
		free(stack->container);
		free(stack);
	}
	return;
}

Generation *stack_peek(const Stack *stack){
	if(stack_is_empty(stack)) return NULL;
	return stack->container[(stack->top - 1 + stack->capacity) % stack->capacity];
}
