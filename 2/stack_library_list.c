#include <stdio.h>
#include <stdlib.h>
#include "stack_library.h"

typedef struct Node {
	char *container;
	struct Node *next;
} Node;

typedef struct Stack {
	Node *top;
	size_t len;
} Stack;

Stack *stack_create(size_t capacity) {
	Stack *stack = (Stack*)calloc(1, sizeof(Stack));
	if (stack == NULL) return NULL;
	stack->top = NULL;
	stack->len = 0;
	return stack;
}

int stack_push(Stack *stack, char *el) {
	Node *new_node = (Node*)malloc(sizeof(Node));
	if (new_node == NULL) return 0;
	new_node->container = el;
	new_node->next = stack->top;
	stack->top = new_node;
	stack->len++;
	return 1;
}

int stack_is_full(const Stack *stack) {
	return 0;
}

int stack_is_empty(const Stack *stack) {
	return stack->top == NULL;
}

char *stack_pop(Stack *stack) {
	if (stack_is_empty) return NULL;
	Node *top_node = stack->top;
	char *container = top_node->container;
	stack->top = top_node->next;
	free(top_node);
	stack->len--;
	return container;
}

void stack_free(Stack *stack){
	Node *current = stack->top;
	while (current != NULL) {
		Node *next = current->next;
		free(current);
		current = next;
	}
	free(stack);
	return;
}
