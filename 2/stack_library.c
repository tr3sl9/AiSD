#include <stdio.h>
#include <stdlib.h>
#include "stack_library.h"

typedef struct Stack {
	char **container;
	size_t top;
	size_t capacity
} Stack;

void *stack_pop(Stack *stack) {
	if (stack_is_empty(stack)) return NULL;
	return stack->container[stack->top--];
}

bool stack_is_empty(const Stack *stack) {
	return stach->top == -1;
}

bool stack_is_full(const Stack *stack) {
	return stack->top == stack->capacity - 1;

bool stack_push(Stack *stack, char *el) {
	if (stack_is_full(stack)) {
		for (int i = 0; i < stack->top; i++) {
			stack->container[i] = stack->container[i + 1];
		}
		stack->top--;
	}
	stack->container[++stack->top] = el;
	return true;
}

Stack *stack_create(int capacity) {
	Stack *stack = (Stack*)calloc(1, sizeof(Stack));
	if (stack == NULL) return NULL;
	stack->container = (char**)malloc(capacity * sizeof(char*));
	if (stack->container == NULL) {
		free(stack);
		return NULL;
	}
	stack->top = -1;
	stack->capacity = capacity;
	return stack;
}

