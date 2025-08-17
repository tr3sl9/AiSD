#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../lib/bst_lib.h"
#include "../client/info_struct.h"

#define insert 1
#define delete 2
#define find 3

#define NUM_SIZES 50
#define NUM_ITER 20

#define NANO 1000000000
#define DEF_COUNT_KEYS 100000
#define ITER_COUNT_KEYS 1000

double time_delta(const struct timespec *start, const struct timespec *end) {
    return end->tv_sec * NANO + end->tv_nsec - start->tv_sec * NANO - start->tv_nsec;
}

void default_insert_tree(BST * const tree) {
    if(!tree) {
        return;
    }

    for (size_t j = 0; j < DEF_COUNT_KEYS; j++) {
        size_t key = rand();
        Info *info = info_create(rand());
        insert_tree(tree, key, info); 
    }

    return;
}

double measuring_time(BST * const tree, size_t *key_arr, Info **info_arr, size_t *idx_arr, int op) {
    struct timespec tp_start;
    struct timespec tp_end;

    switch (op) {
        case insert:
            clock_gettime(CLOCK_MONOTONIC, &tp_start);
            for (size_t j = 0; j < ITER_COUNT_KEYS; j++) {
                insert_tree(tree, key_arr[j], info_arr[j]);
            }
            clock_gettime(CLOCK_MONOTONIC, &tp_end);
            return time_delta(&tp_start, &tp_end);

        case find:
            clock_gettime(CLOCK_MONOTONIC, &tp_start);
            for (size_t j = 0; j < ITER_COUNT_KEYS; j++) {
                find_tree(tree, key_arr[idx_arr[j]]);
            }
            clock_gettime(CLOCK_MONOTONIC, &tp_end);
            return time_delta(&tp_start, &tp_end);

        case delete:
            clock_gettime(CLOCK_MONOTONIC, &tp_start);
            for (size_t j = 0; j < ITER_COUNT_KEYS; j++) {
                delete_tree(tree, key_arr[j]);
            }
            clock_gettime(CLOCK_MONOTONIC, &tp_end);
            return time_delta(&tp_start, &tp_end);
    }
    return 0;
}

double iteration_insert_tree(BST * const tree, size_t *key_arr, Info **info_arr) {
    if (!tree || !key_arr || !info_arr) {
        return 0;
    }
    for (size_t j = 0; j < ITER_COUNT_KEYS; j++) {
        info_arr[j] = info_create(rand());
        key_arr[j] = rand();
    }
    return measuring_time(tree, key_arr, info_arr, NULL, insert);
}

double iteration_find_tree(BST * const tree, size_t *key_arr, size_t *idx_arr) {
    if (!tree || !key_arr || !idx_arr) {
        return 0;
    }
    for (size_t j = 0; j < ITER_COUNT_KEYS; j++) {
        idx_arr[j] = rand() % ITER_COUNT_KEYS;
    }
    return measuring_time(tree, key_arr, NULL, idx_arr, find);
}

double iteration_delete_tree(BST * const tree, size_t *key_arr) {
    if (!tree || !key_arr) {
        return 0;
    }
    return measuring_time(tree, key_arr, NULL, NULL, delete);
}

int main() {
    FILE *file = fopen("bst_benchmark.csv", "w");

    BST *tree = create_tree();
    if (!tree) {
        printf("Memory allocation failed\n");
        return 1;
    }
    
    fprintf(file, "TreeSize;InsertTime;SearchTime;DeleteTime\n");
    
    for (size_t s = 0; s < NUM_SIZES; s++) {

        default_insert_tree(tree);

        double time_insert = 0;
        double time_delete = 0;
        double time_search = 0;

        for (size_t i = 0; i < NUM_ITER; i++) {
            size_t *key_arr = malloc(ITER_COUNT_KEYS * sizeof(size_t));
            if (!key_arr) {
                printf("Memory allocation failed\n");
                free_tree(tree);
                fclose(file);
                return 1;
            }

            Info **info_arr = malloc(ITER_COUNT_KEYS * sizeof(Info*));
            if (!info_arr) {
                printf("Memory allocation failed\n");
                free(key_arr);
                free_tree(tree);
                fclose(file);
                return 1;
            }

            // Вставка
            time_insert += iteration_insert_tree(tree, key_arr, info_arr);
            // Поиск
            size_t *idx_arr = malloc(ITER_COUNT_KEYS * sizeof(size_t));
            if (!idx_arr) {
                printf("Memory allocation failed\n");
                free(info_arr);
                free(key_arr);
                free_tree(tree);
                fclose(file);
                return 1;
            }

            time_search += iteration_find_tree(tree, key_arr, idx_arr);
            // Удаление
            time_delete += iteration_delete_tree(tree, key_arr);

            free(info_arr);
            free(key_arr);
            free(idx_arr);
        }
        // Записать средние значения по размеру дерева
        fprintf(file, "%zu;%.6f;%.6f;%.6f\n", (s + 1) * DEF_COUNT_KEYS, time_insert/(double)NUM_ITER, time_search/(double)NUM_ITER, time_delete/(double)NUM_ITER);
        printf("Тестирование для дерева с %zu ключами завершено.\n", (s + 1) * DEF_COUNT_KEYS);
    }
    
    free_tree(tree);
    fclose(file);
    printf("Все тесты завершены. Результаты сохранены в bst_benchmark.csv\n");
    return 0;
}

