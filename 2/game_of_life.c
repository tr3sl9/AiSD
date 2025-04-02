#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_of_life.h"

#define MAGIC_WORD "GAME_OF_LIFE\n"

Generation *create_gen(size_t width, size_t height) {
        Generation *gen = (Generation*)malloc(sizeof(Generation));
        if (gen == NULL) return NULL;
        gen->field = create_field(width, height);
        if (gen->field == NULL) {
                free(gen);
                return NULL;
        }
        gen->width = width;
        gen->height = height;
        return gen;
}

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
                memset(field[i], die, width);
        }
        return field;
}

Generation *copy_gen(const Generation *current) {
        if (current == NULL || current->height == 0 || current->width == 0) return NULL;
        Generation *copy = create_gen(current->width, current->height);
        for (size_t i = 0; i < current->height; i++) {
                memcpy(copy->field[i], current->field[i], current->width);
        }
        return copy;
}

Generation *load_initial_state(const char *filename) {
        if (filename == NULL) return NULL;
        FILE *file = fopen(filename, "r");
        if (file == NULL) {
                fprintf(stderr, "Ошибка открытия файла\n");
                return NULL;
        }
        char magic_word[sizeof(MAGIC_WORD)] = {0};
        fgets(magic_word, sizeof(magic_word), file);
        if (strcmp(magic_word, MAGIC_WORD) != 0) {
                fclose(file);
                fprintf(stderr, "Ошибка, неверный формат данный в файле\n");
                return NULL;
        }
        size_t width, height;
        if (fscanf(file, "%zu %zu", &width, &height) != 2 || width == 0 || height == 0) {
                fclose(file);
                fprintf(stderr, "Ошибка в прочтении размерности игрового поля\n");
                return NULL;
        }

        while (fgetc(file) != '\n' && !feof(file));

        Generation *gen = create_gen(width, height);
        if (gen == NULL) {
                fclose(file);
                fprintf(stderr, "Ошибка генерации игрового поля\n");
                return NULL;
        }
        char *line = (char*)malloc((width + 2) * sizeof(char));
        size_t row = 0;
        while (fgets(line, width + 2, file)) {
                if (strlen(line) < width) {
                        fprintf(stderr, "Строка %zu слишком короткая\n", row + 1);
                        free(line);
                        free_gen(gen);
                        fclose(file);
                        return NULL;
                }
                for(size_t i = 0; i < width; i++) {
                        gen->field[row][i] = line[i];
                }
                row++;
        }
        free(line);
        fclose(file);
        if (row != height) {
                fprintf(stderr, "Недостаточно строк в файле\n");
                free_gen(gen);
                return NULL;
        }
        return gen;
}

int count_neighbour(const Generation *current, ssize_t i, ssize_t j) {
        if (current == NULL || current->field == NULL) return 0;
        size_t count_of_live = 0;
        for(ssize_t k = i - 1; k < i + 2; k++) {
                for(ssize_t l = j - 1; l < j + 2; l++) {
                        if (k == i && l == j) continue;
                        if (current->field[(k + current->height) % (current->height)][(l + current->width) % (current->width)] == live) count_of_live++;
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

Generation *next_gen(const Generation *current) {
        if (current == NULL || current->field == NULL) return NULL;
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

void print_gen(Generation *current) {
        if (current == NULL) {
                printf("Генерация пуста\n");
                return;
        }
        for(size_t i = 0; i < current->height; i++) {
                for(size_t j = 0; j < current->width; j++) {
                        printf("%c", current->field[i][j]);
                }
                printf("\n");
        }
        return;
}

int save_state_to_file(const char *filename, const Generation *gen) {
        if (filename == NULL || gen == NULL || gen->field == NULL) {
                fprintf(stderr, "Ошибка: NULL параметры\n");
                return 0;
        }
        if (gen->width <= 0 || gen->height <= 0) {
                fprintf(stderr, "Ошибка: неверные размеры поля (%zu x %zu)\n", gen->width, gen->height);
        return 0;
    }
        FILE *file = fopen(filename, "w");
        if (file == NULL) {
                fprintf(stderr, "Ошибка открытия файла\n");
                return 0;
        }
        if (fprintf(file, "GAME_OF_LIFE\n%zu %zu\n", gen->width, gen->height) < 0) {
        fclose(file);
        return 0;
    }
        for(size_t i = 0; i < gen->height; i++) {
                for(size_t j = 0; j < gen->width; j++) {
                         if (gen->field[i][j] != live && gen->field[i][j] != die) {
                fprintf(stderr, "Ошибка: недопустимый символ '%c' в позиции [%zu][%zu]\n", gen->field[i][j], i, j);
                fclose(file);
                return 0;
            }
            if (fputc(gen->field[i][j], file) == EOF) {
                fprintf(stderr, "Ошибка записи символа поля\n");
                fclose(file);
                return 0;
            }
        }
        if (fputc('\n', file) == EOF) {
            fprintf(stderr, "Ошибка записи перевода строки\n");
            fclose(file);
            return 0;
        }
    }
    if (fclose(file) == EOF) {
        fprintf(stderr, "Ошибка закрытия файла");
        return 0;
    }
        return 1;
}

void free_field(char **field, size_t height) {
        for(size_t i = 0; i < height; i++) {
                free(field[i]);
        }
        free(field);
        return;
}

void free_gen(Generation *gen){
        if (gen) {
                free_field(gen->field, gen->height);
                free(gen);
        }
        return;
}
