#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_of_life.h"
#include "stack_library.h"

char controls() {
	char input;
	printf("Нажмите: \n'n' для перехода к следующей генерации \n'p' для перехода к предыдущей генерации \n'q' для выхода \n'v' для просмотра текущей генерации\n");
	scanf("%c", &input);
	return input;
}

void go_next(Generation *current, Generation *next, Stack *history) {
	next = next_gen(current);
	stack_push(history, next);
	current = next;
	next = create_gen(current->width, current->height);
	return;
}

void go_previous(Generation *current, Generation *next, Stack *history) {
	if (stack_is_empty(history)) {
		printf("Предыдущая генерация недоступна\n");
	} else {
		free_gen(next);
		next = current;
		current = (Generation*)stack_pop(history);
	}
	return;
}

void print_current(Stack *history) {
	Generation *peek = (Generation*)stack_peek(history);
	if (peek == NULL) {
		printf("Генераций еще не было\n");
	} else {
		printf("Текущая генерация\n");
		print_gen(peek);
	}
	return;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Ошибка в названии файла\n");
		return OK;
	}
	Stack *history = stack_create(10);
	Generation *current = load_initial_state(argv[1]);
	if (current == NULL) {
		stack_free(history);
		return OK;
	}
	Generation *next = next_gen(current);
	if (next == NULL) {
		free_gen(current);
		stack_free(history);
	}
	stack_push(history, current);
	char input;
	while (1) {
		print_gen(current);
		input = controls();
		if (input == 'n') go_next(current, next, history);
		else if (input == 'p') go_previous(current, next, history);
		else if (input == 'v') print_current(history);
		else if (input == 'q') break;
	}
	while (!stack_is_empty(history)) {
		free_gen((Generation*)stack_pop(history));
	}
	stack_free(history);
	free_gen(next);
	return 0;
}
