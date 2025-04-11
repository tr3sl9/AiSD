#include <stdio.h>
#include <stdlib.h>
#include "stack_library.h"
#include "game_of_life.h"

typedef struct Node {
	Generation *container;
	struct Node *next;
} Node;

typedef struct Stack {
	Node *head;
} Stack;

Stack *stack_create() {
	Stack *stack = (Stack*)calloc(1, sizeof(Stack));
	if (stack == NULL) return NULL;
	return stack;
}

Node *create_node() {
	return (Node*)calloc(1, sizeof(Node));
}

state stack_push(Stack * const stack, Generation * const el) {
	Node *new_node = create_node();
	if (new_node == NULL) {
		return EMPTY;
	}
	new_node->container = el;
	new_node->next = stack->head;
	stack->head = new_node;
	return OK;
}

state stack_is_empty(const Stack * const stack) {
	if (stack->head == NULL) return EMPTY;
	return OK;
}

state stack_pop(Stack * const stack, Generation ** const current) {
	if (stack_is_empty(stack) == EMPTY) return EMPTY;
	Node *head_node = stack->head;
	Generation *container = head_node->container;
	stack->head = head_node->next;
	free(head_node);
	*current = container;
	return OK;
}

void stack_free(Stack * const stack){
	if (stack == NULL) {
		return;
	}
	Node *current = stack->head;
	while (current != NULL) {
		Node *next = current->next;
		free_gen(current->container);
		free(current);
		current = next;
	}
	free(stack);
	return;
}

Generation *stack_peek(const Stack * const stack) {
	if (stack_is_empty(stack) == EMPTY) return NULL;
	Generation *copy = copy_gen(stack->head->container);
	return copy;
}

