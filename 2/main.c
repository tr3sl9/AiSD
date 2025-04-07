#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "game_of_life.h"
#include "stack_library.h"

char controls() {
    char input;
    char new_line = ' ';
    printf("Нажмите: \n'n' для перехода к следующей генерации \n'p' для перехода к предыдущей генерации \n'q' для выхода \n'v' для просмотра текущей генерации\n'a' для автоматической игры\n");
    if (scanf("%c", &input) == EOF) {
		return 'e';
	}
    while (new_line != '\n') {
        scanf("%c", &new_line);
    }
    return input;
}

Generation *go_next(Generation *current, Stack *history) {
    if (stack_push(history, current) != OK) {
		return NULL;
	}
    Generation *next = next_gen(current);
	if (next == NULL) {
		return NULL;
	}
    return next;
}

Generation *go_previous(Stack *history) {
    if (stack_is_empty(history) == EMPTY) {
        printf("Предыдущая генерация недоступна\n");
        return NULL;
    } else {
        Generation *current;
        stack_pop(history, &current);
        return current;
    }
}

void print_current(Generation *current) {
    if (current == NULL) {
        printf("Генераций еще не было\n");
    } else {
        printf("Текущая генерация:\n");
        print_gen(current);
    }
    return;
}

void break_game(Generation *current, const char *filename) {
    if(save_state_to_file(filename, current)) {
        printf("Генерация успешно сохранена\n");
    } else {
        fprintf(stderr, "Ошибка сохранения генерации в файл\n");
    }
    return;
}

void handle_invalid_input()	{
	printf("Такой команды нет. Выбиерите команду из списка\n");
	return;
}

int break_signal = 0;

void handle_sigint(int sig __attribute__((__unused__))) {
	break_signal = 1;
	return;
}

void auto_play(Generation **current, Stack *history) {
	signal(SIGINT, handle_sigint);
	while (!break_signal) {
		*current = go_next(*current, history);
		print_gen(*current);
		sleep(1);
	}
	break_signal = 0;
	signal(SIGINT, SIG_DFL);
	return;
}

int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "Ошибка в названии файла\n");
		return 0;
	}
	Stack *history = stack_create(atoi(argv[2]));
	if (history == NULL) {
		return EMPTY;
	}
	Generation *current = load_initial_state(argv[1]);
	if (current == NULL) {
		stack_free(history);
		return EMPTY;
	}
	char input;
	print_gen(current);
	while (1) {
		input = controls();
		if (input == 'e') {
			break;
		} 
		else if (input == 'n') {
			current = go_next(current, history);
			print_gen(current);
		}
		else if (input == 'p') {
			Generation *gen;
			gen = go_previous(history);
			if (gen != NULL) {
				free_gen(current);
				current = gen;
				print_gen(current);
			}
		}
		else if (input == 'v') {
			print_current(current);
		}
		else if (input == 'q') {
			break_game(current, argv[1]);
			break;
		}
		else if (input == 'a') {
			auto_play(&current, history);
		} else {
			handle_invalid_input();	
		}
	}
	stack_free(history);
	free_gen(current);
	return OK;
}
