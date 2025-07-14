#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../lib/bst_lib.h"
#include "../client/info_struct.h"

#define NUM_SIZES 30
#define NUM_ITER 20
#define NUM_OPS 1000

#define NANO 1000000000
#define DEF_COUNT_KEYS 40000
#define ITER_COUNT_KEYS 1000

double time_delta(const struct timespec *start, const struct timespec *end) {
    return end->tv_sec * NANO + end->tv_nsec - start->tv_sec * NANO - start->tv_nsec;
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

        //заполняю на 90 000 каждый раз
        for (size_t j = 0; j < DEF_COUNT_KEYS; j++) {
            size_t key = rand();
            Info *info = info_create(rand());
            insert_tree(tree, key, info); 
        }

        double time_insert = 0;
        double time_delete = 0;
        double time_search = 0;

        for (size_t i = 0; i < NUM_ITER; i++) {
            size_t *keys = malloc(ITER_COUNT_KEYS * sizeof(size_t));
            if (!keys) {
                printf("Memory allocation failed\n");
                fclose(file);
                return 1;
            }
            // Вставка
            struct timespec tp_start;
            struct timespec tp_end;
            clock_gettime(CLOCK_MONOTONIC, &tp_start);
            for (size_t j = 0; j < ITER_COUNT_KEYS; j++) {
                size_t key = rand();
                Info *info = info_create(rand());
                keys[j] = key;
                insert_tree(tree, key, info); 
            }
            clock_gettime(CLOCK_MONOTONIC, &tp_end);
            time_insert += time_delta(&tp_start, &tp_end);
            // Поиск
            clock_gettime(CLOCK_MONOTONIC, &tp_start);
            for (size_t j = 0; j < ITER_COUNT_KEYS; j++) {
                int idx = rand() % ITER_COUNT_KEYS;
                find_tree(tree, keys[idx]);
            }
            clock_gettime(CLOCK_MONOTONIC, &tp_end);
            time_search += time_delta(&tp_start, &tp_end);
            // Удаление
            clock_gettime(CLOCK_MONOTONIC, &tp_start);
            for (size_t j = 0; j < ITER_COUNT_KEYS; j++) {
                delete_tree(tree, keys[j]);
            }
            clock_gettime(CLOCK_MONOTONIC, &tp_end);
            time_delete += time_delta(&tp_start, &tp_end);

            free(keys);
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

