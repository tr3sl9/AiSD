#include <stdio.h>
#include <stdlib.h>
#include "stack_library.h"
#include "game_of_life.h"

typedef struct Node {
	Generation *container;
	struct Node *next;
} Node;

typedef struct Stack {
	Node *top;
	size_t current_len;
	size_t capacity;
} Stack;

Stack *stack_create(size_t capacity) {
	Stack *stack = (Stack*)calloc(1, sizeof(Stack));
	if (stack == NULL) return NULL;
	stack->top = NULL;
	stack->current_len = 0;
	stack->capacity = capacity;
	return stack;
}

state stack_push(Stack *stack, Generation *el) {
	if (stack == NULL || el == NULL) {
		return NOK;
	}
	if (stack_is_full(stack)) {
		if (stack->current_len == 1) {
			free_gen(stack->top->container);
			stack->top->container = el;
			return OK;
		}
		Node *current = stack->top;
		while (current->next != NULL && current->next->next != NULL) {
			current = current->next;
		}
		if (current->next) {
			free_gen(current->next->container);
			free(current->next);
			current->next = NULL;
			stack->current_len--;
		}
	}
	Node *new_node = (Node*)malloc(sizeof(Node));
	if (new_node == NULL) {
		return NOK;
	}
	new_node->container = el;
	new_node->next = stack->top;
	stack->top = new_node;
	stack->current_len++;
	return OK;
}

state stack_is_full(const Stack *stack) {
	if (stack == NULL) return NOK;
	return stack->current_len == stack->capacity - 1;
}

state stack_is_empty(const Stack *stack) {
	if (stack == NULL) return NOK;
	if (stack->top == NULL) return OK;
	return NOK;
}

Generation *stack_pop(Stack *stack) {
	if (stack_is_empty(stack)) return NULL;
	Node *top_node = stack->top;
	Generation *container = top_node->container;
	stack->top = top_node->next;
	free(top_node);
	stack->current_len--;
	return container;
}

void stack_free(Stack *stack){
	if (stack == NULL) {
		return;
	}
	Node *current = stack->top;
	while (current != NULL) {
		Node *next = current->next;
		free_gen(current->container);
		free(current);
		current = next;
	}
	free(stack);
	return;
}

Generation *stack_peek(const Stack *stack){
	if (stack_is_empty(stack)) return NULL;
	return stack->top->container;
}
