#include <stdio.h>
#include <stdlib.h>
#include "stack_library.h"

typedef struct Stack {
	char **container;
	size_t head;
	size_t tail;
	size_t capacity;
	size_t current_len;
} Stack;

char *stack_pop(Stack *stack) {
	if (stack_is_empty(stack)) return NULL;
	char *el = stack->container[stack->head];
	stack->head = (stack->head + 1) % stack->capacity;
	stack->current_len--;
	return el;
}

state stack_is_empty(const Stack *stack) {
	return stack->head == NOK;
}

state stack_is_full(const Stack *stack) {
	return stack->current_len == stack->capacity - 1;
}

state stack_push(Stack *stack, void *el) {
	if (stack_is_full(stack)) {
		stack->head = (stack->head + 1) % stack->capacity;
		stack->current_len--;
	}
	stack->container[stack->tail] = el;
	stack->tail = (stack->tail + 1) % stack->capacity;
	stack->current_len++;
	return OK;
}

Stack *stack_create(size_t capacity) {
	Stack *stack = (Stack*)calloc(1, sizeof(Stack));
	if (stack == NULL) return NULL;
	stack->container = (char**)malloc(capacity * sizeof(char*));
	if (stack->container == NULL) {
		free(stack);
		return NULL;
	}
	stack->head = 0;
	stack->tail = 0;
	stack->current_len = 0;
	stack->capacity = capacity;
	return stack;
}

void stack_free(Stack *stack){
	if (stack) {
		free(stack->container);
		free(stack);
	}	
	return;
}

char *stack_peek(const Stack *stack){
	if(stack_is_empty(stack)) return NULL;
	return stack->container[stack->head];
}
