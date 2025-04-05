#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_of_life.h"
#include "stack_library.h"

char controls() {
    char input;
    char new_line = ' ';
    printf("Нажмите: \n'n' для перехода к следующей генерации \n'p' для перехода к предыдущей генерации \n'q' для выхода \n'v' для просмотра текущей генерации\n");
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

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Ошибка в названии файла\n");
		return 0;
	}
	Stack *history = stack_create(10);
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
			free_gen(current);
			current = go_previous(history);
			if (current != NULL) {
				print_gen(current);
			}
		}
		else if (input == 'v') {
			print_current(current);
		}
		else if (input == 'q') {
			break_game(current, argv[1]);
			break;
		} else {
			handle_invalid_input();	
		}
	}
	stack_free(history);
	free_gen(current);
	return OK;
}
