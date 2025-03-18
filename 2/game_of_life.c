#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_of_life.h"

char **create_field(size_t width, size_t height) {
	char **field = (char**)malloc(height * sizeof(char*));
	if (field == NULL) return NULL;
	for(size_t i = 0; i < height; i++) {
		field[i] = (char*)calloc(width, sizeof(char));
		if (field[i] == NULL) {
			for(size_t j = 0; j < i; j++){
				free(field[j]);
			}
			free(field);
			return NULL;
		}
	}
	return field;
}

void free_field(char **field, size_t height) {
	for(size_t i = 0; i < height; i++) {
		free(field[i]);
	}
	free(field);
	return;
}

Generation *create_gen(size_t width, size_t height) {
	Generation *gen = (Generation*)malloc(sizeof(Generation));
	if (gen == NULL) return NULL;
	gen->field = create_field(width, height);
	if (gen->field == NULL) return NULL;
	gen->width = width;
	gen->height = height;
	return gen;
}

void free_gen(Generation *gen){
	if (gen) {
		free_field(gen->field, gen->height);
		free(gen);
	}
}

void print_gen(Generation *current) {
	if (current == NULL) {
		printf("Генерация пуста\n");
		return;
	}
	for(int i = 0; i < current->height; i++) {
		for(int j = 0; j < current->width; j++) {
			printf("%c", current->field[i][j]);
		}
		printf("/n");	
	}
	return;
}

int count_neighbour(Generation *current, size_t i, size_t j) {
	size_t count_of_live = 0;
	for(ssize_t k = i - 1; k < i + 2; k++) {
		for(ssize_t l = j - 1; l < j + 2; j++) {
			if (k == i && l == j) continue;
			if (current->field[k][l] == live) count_of_live++;
		}
	}
	return count_of_live;
}

cell_state change_state(char cell, size_t count_of_live) {
	if (cell == live) {
		if (count_of_live == 2 || count_of_live == 3) {
			return live;
		} else {
			return die;
		}
	} else {
		if (count_of_live == 3) {
			return live;
		} else {
			return die;
		}
	}
}

Generation *next_gen(Generation *current) {
	Generation *next = create_gen(current->width, current->height);
	if (next == NULL) return NULL;
	for(size_t i = 0; i < current->height; i++) {
		for(size_t j = 0; j < current->width; j++) {
			int count_of_live = count_neighbour(current, i, j);
			next->field[i][j] = change_state(current->field[i][j], count_of_live);
		} 
	}
	return next;
}

Generation *load_initial_state(const char *filename) {
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "Ошибка открытия файла\n");
		return NULL;
	}
	char magic_word[12];
	fgets(magic_word, sizeof(magic_word), file);
	if (strcmp(magic_word, "GAME_OF_LIFE\n") != 0) {
		fclose(file);
		fprintf(stderr, "Ошибка, неверный формат данный в файле\n");
		return NULL;
	}
	int width, height;
	if (fscanf(file, "%d %d", &width, &height) != 2) {
		fclose(file);
		fprintf(stderr, "Ошибка в прочтении размерности игрового поля\n");
		return NULL;
	}
	while (fgetc(file) != '\n');
	Generation *gen = create_gen(width, height);
	if (gen == NULL) {
		fclose(file);
		fprintf(stderr, "Ошибка генерации игрового поля\n");
		return NULL;
	}
	char *line = (char*)malloc(width * sizeof(char));
	int row = 0;
	while (fgets(line, sizeof(line), file)) {
		if (row >= height) break;
		for(int i = 0; i < width; i++) {
			gen->field[row][i] = line[i];
		}
		row++;
	}
	fclose(file);
	return gen;
}

//TODO Выгрузка в файл для сохранения при выходе
