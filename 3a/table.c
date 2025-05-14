#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "table.h"
#include "my_struct.h"

#define MAGIC_WORD "TABLE\n"

void print_ks(const KeySpace * const ks, const size_t i) {
    printf("| %-5zu | %-30s | %-9zu | %-20s |\n", i, ks->key, ks->release, ks->info);
    return;
}

static int cmp(const char * const str_1, const char * const str_2) {
    return strcmp(str_1, str_2);
}

static int not_same_ks(const KeySpace * const ks1, const KeySpace * const ks2) {
    if (ks1 == NULL || ks2 == NULL) {
        return 0;
    }

    return !(cmp(ks1->key, ks2->key) == 0 && ks1->release == ks2->release && cmp(ks1->info, ks2->info) == 0);
}

static void set_ks(KeySpace * const ks, const char * const key, const char * const info, const size_t release) {
    if (!ks || !key || !info) {
        return;
    }

    ks->key = strdup(key);
    if (release != 0) {
        ks->release = release;
    }

    ks->info = strdup(info);

    return;
}

Table *create_table(const size_t msize) {
    Table *table = (Table*)calloc(1, sizeof(Table));
    if (table == NULL) {
        return NULL;
    }

    table->ks = (KeySpace*)calloc(msize, sizeof(KeySpace));
    if (table->ks == NULL) {
        free(table);
        return NULL;
    }

    table->msize = msize;
    return table;
}

static void free_ks(KeySpace * const ks) {
    if (!ks) {
        return;
    }

    free(ks->key);
    free(ks->info);
    return;
}

static size_t find_max_release(const Table * const table, const char * const key) {
    size_t release = 0;
    for (size_t i = 0; i < table->csize; i++) {
        if (cmp(table->ks[i].key, key) == 0 && table->ks[i].release > release) {
            release = table->ks[i].release;
        } 
    }

    return release;
}

static void rm_element_with_move(KeySpace * const current_ks, KeySpace * const last_ks) {
    if (not_same_ks(current_ks, last_ks)) {
        free_ks(current_ks);
        set_ks(current_ks, last_ks->key, last_ks->info, last_ks->release);
    }

    free_ks(last_ks);
    return;
}

void free_table(Table * const table) {
    for (size_t i = 0; i < table->csize; i++) {
        free_ks(table->ks + i);
    }

    free(table->ks);
    free(table);
    return;
}

table_err insert_key_to_table(Table * const table, const char * const key, const char * const info) {
    if (!table) {
        return TABLE_NULL;
    }
    if (!key || !info) {
        return TABLE_VAL;
    }
    if (table->csize >= table->msize) {
        return TABLE_FULL;
    }

    size_t release = find_max_release(table, key);
    set_ks(table->ks + table->csize, key, info, release + 1);
    table->csize++;
    return TABLE_OK;
}

static DynamicArray *find_elements(const Table * const table, const char * const key) {
    DynamicArray *da = da_create(sizeof(KeySpace*));
    if (!da) {
        return NULL;
    }

    for (size_t i = 0; i < table->csize; i++) {
        if (cmp(table->ks[i].key, key) == 0) {
            KeySpace *ptr = table->ks + i;
            if (!da_append(da, &ptr)) {
                da_free(da);
                return NULL;
            }
        }
    }

    return da;
}

static KeySpace *find_element_with_release(const Table * const table, const char * const key, const size_t release) {
    for (size_t i = 0; i < table->csize; i++) {
        if (cmp(table->ks[i].key, key) == 0 && table->ks[i].release == release) {
            return table->ks + i;
        }
    }

    return NULL;
}

table_err delete_key_from_table(Table * const table, const char * const key) {
    if (!table) {
        return TABLE_NULL;
    }
    if (!key) {
        return TABLE_VAL;
    }

    DynamicArray *da = find_elements(table, key);
    if (!da) return TABLE_MEM;

    if (da->count == 0) {
        da_free(da);
        return TABLE_VAL;
    }

    for (size_t i = da->count; i > 0; i--) {
        KeySpace **ks_ptr = (KeySpace **)((char *)da->array + (i - 1) * da->size_of_one);
        rm_element_with_move(*ks_ptr, table->ks + table->csize - 1);
        if (table->csize != 0) table->csize--;    
    }

    da_free(da);
    return TABLE_OK;
}

table_err print_table(const Table * const table) {
    if (!table) {
        return TABLE_NULL;
    }
    printf("\nTable contents (%zu/%zu):\n", table->csize, table->msize);
    printf("┌───────┬────────────────────────────────┬───────────┬──────────────────────┐\n");
    printf("│ Index │ Key                            │ Release   │ Info                 │\n");
    printf("├───────┼────────────────────────────────┼───────────┼──────────────────────┤\n");
    for (size_t i = 0; i < table->csize; i++) {
        print_ks(table->ks + i, i);
        if (i < table->csize - 1) {
            printf("├───────┼────────────────────────────────┼───────────┼──────────────────────┤\n");
        }
    }
    printf("└───────┴────────────────────────────────┴───────────┴──────────────────────┘\n");

    return TABLE_OK;
}

static void realloc_attribute(char **atr, size_t *capacity) {
    (*capacity) *= 2;
    *atr = (char*)realloc(*atr, *capacity * sizeof(char));
    return;
}

static void create_attribute(char **atr, size_t *capacity) {
    *capacity = BUFSIZ;
    *atr = (char*)malloc(*capacity * sizeof(char));
    return;
}

static table_err read_row(char **atr, FILE *file, size_t *capacity) {
    ssize_t c;
    size_t current_len = 0;
    if (!atr || !*atr || !capacity) {
        return TABLE_VAL;
    }

    while((c = fgetc(file)) != EOF && c != '\n') {
        if (current_len + 1 >= *capacity) {
            realloc_attribute(atr, capacity);
            if (!*atr) {
                return TABLE_MEM;
            }
        }
        (*atr)[current_len++] = (char)c;
    }

    if (current_len == 0 && c == EOF) {
        return TABLE_EOF;
    }
    if (current_len + 1 >= *capacity) {
        realloc_attribute(atr, capacity);
    }
    if (!*atr) {
        return TABLE_MEM;
    }

    (*atr)[current_len] = '\0';
    return TABLE_OK;
}

table_err import_table_from_file(Table * const table, const char * const filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return FILE_ERR;
    }

    char magic_word[sizeof(MAGIC_WORD)] = {0};
    fgets(magic_word, sizeof(magic_word), file);
    if (cmp(magic_word, MAGIC_WORD) != 0) {
        fclose(file);
        return TABLE_MAGIC_WORD;
    }

    size_t msize, csize;
    if (fscanf(file, "%zu %zu\n", &msize, &csize) != 2) {
        fclose(file);
        return TABLE_SIZE;
    }
    if (csize > msize || msize == 0) {
        fclose(file);
        return TABLE_SIZE;
    }

    char empty_line[2];
    fgets(empty_line, sizeof(empty_line), file);
    size_t key_capacity = 0, info_capacity = 0;
    char *key = NULL, *info = NULL;
    ssize_t release;

    create_attribute(&key, &key_capacity);
    if (!key) {
        fclose(file);
        return TABLE_MEM;
    }    
    create_attribute(&info, &info_capacity);
    if (!info) {
        free(key);
        fclose(file);
        return TABLE_MEM;
    }

    table_err result = TABLE_OK;
    while (table->csize <= table->msize) {
        result = read_row(&key, file, &key_capacity);
        if (result != TABLE_OK) {
            break;
        }
        if (fscanf(file, "%zd\n", &release) != 1 || release < 1) {
            result = TABLE_VAL;
            break;
        }
        result = read_row(&info, file, &info_capacity);
        if (result != TABLE_OK) {
            break;
        }
        fgets(empty_line, sizeof(empty_line), file);
        set_ks(table->ks + table->csize, key, info, release);
        table->csize++;
    }

    free(key);
    free(info);
    fclose(file);
    return result;
}

table_err export_table_to_file(const Table * const table, const char * const filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        return FILE_ERR;
    }

    fprintf(file, "%s", MAGIC_WORD);
    fprintf(file, "%zu %zu\n", table->msize, table->csize);
    for (size_t i = 0; i < table->csize; i++) {
        fprintf(file, "\n");
        fprintf(file, "%s\n%zu\n%s\n", table->ks[i].key, table->ks[i].release, table->ks[i].info);
    }

    fclose(file);
    return TABLE_OK;
}

Table* search_by_key_in_table(const Table * const table, const char * const key) {
    if (!table || !key) {
        return NULL;
    }

    DynamicArray *da = find_elements(table, key);
    if (!da) {
        return NULL;
    }
    
    Table *result = create_table(da->count);
    if (!result) {
        da_free(da);
        return NULL;
    }
    
    for (size_t i = 0; i < da->count; i++) {
        KeySpace *ks = *(KeySpace **)((char *)da->array + i * da->size_of_one);
        set_ks(result->ks + result->csize, ks->key, ks->info, ks->release);
        result->csize++;
    }
    
    da_free(da);
    return result;
}

Table* search_by_key_with_release_in_table(const Table * const table, const char * const key, const size_t release) {
    if (!table || !key) {
        return NULL;
    }

    Table *result = create_table(1);
    if (!result) {
        return NULL;
    }

    KeySpace *ks = find_element_with_release(table, key, release);
    if (!ks) {
        return NULL;
    }

    set_ks(result->ks, ks->key, ks->info, ks->release);
    result->csize++;

    return result;
}

static size_t find_max_release_in_da(const DynamicArray * const da) {
    size_t max_release = 0;
    for (size_t i = 0; i < da->count; i++) {
        KeySpace *ks = *(KeySpace **)((char *)da->array + i * da->size_of_one);
        if (ks->release > max_release) {
            max_release = ks->release;
        }
    }

    return max_release;
}

static void delete_element_with_non_max_releases(Table * const table, DynamicArray * const  da, const size_t max_release) {
    for (size_t j = 0; j < da->count; j++) {
        KeySpace *ks = *(KeySpace **)((char *)da->array + j * da->size_of_one);
        if (ks->release < max_release) {
            rm_element_with_move(ks, table->ks + table->csize - 1);
            if (table->csize > 0) table->csize--;
        }
    }
    return; 
}

table_err clean_table(Table * const table) {
    if (!table) {
        return TABLE_NULL;
    }
    if (table->csize == 0) {
        return TABLE_OK;
    }

    for (size_t i = 0; i < table->csize; i++) {
        DynamicArray *da = find_elements(table, table->ks[i].key);
        size_t max_release = find_max_release_in_da(da);
        delete_element_with_non_max_releases(table, da, max_release); 
        da_free(da);
    }

    return TABLE_OK;
}
