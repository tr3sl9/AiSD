#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "table.h"
#include "../client/da_library.h"
#include "../client/info_struct.h"

#define HASH_SEED 0x5bd1e995
#define MAGIC_WORD "HASH_TABLE\n"

static size_t hash1(const char * const key, const size_t msize) {
    size_t hash = SIZE_MAX;
    for (size_t i = 0; key[i] != '\0'; i++) {
        hash = 37 * hash + key[i];
    }
    return hash > 0 ? hash % msize : -(hash) % msize;
}

// MurmurHash
static size_t hash2(const void * const key) {
    const size_t seed = HASH_SEED;
    const size_t m = HASH_SEED;
    size_t len = strlen(key);
    const int r = 24;
    size_t h = seed ^ len;
    const unsigned char *data = (const unsigned char *)key;
    while(len >= sizeof(size_t)) {
        size_t k;
        memcpy(&k, data, sizeof(size_t));
        k *= m;
        k ^= k >> r;
        k *= m;
        h *= m;
        h ^= k;
        data += sizeof(size_t);
        len -= sizeof(size_t);
    }
    switch(len) {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
                h *= m;
    };
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h;
}

int cmp(const char * const str_1, const char * const str_2) {
    return strcmp(str_1, str_2);
}

void print_ks(const KeySpace * const ks, const char * const status, const size_t i) {
    if (!ks->key || !ks->info) {
        printf("| %-5zu | %-8s | %-30s | %-20d | %-9d |\n", i, "EMPTY", "NULL", 0, 0);
        return;
    }
    printf("| %-5zu | %-8s | %-30s | %-20zu | %-9zu |\n", i, status, ks->key, ks->info->info, ks->release);
    return;
}

Table *create_table(const size_t msize) {
    Table *table = (Table*)calloc(1, sizeof(Table));
    if (!table) {
        return NULL;
    }

    table->ks = (KeySpace*)calloc(msize, sizeof(KeySpace));
    if (!table->ks) {
        free(table);
        return NULL;
    }

    table->msize = msize;
    return table;
}

void free_ks(KeySpace * const ks) {
    ks->busy = DELETED;
    free(ks->key);
    ks->key = NULL;
    info_free(ks->info);
    return;
}

void free_table(Table * const table) {
    if (!table) return;

    for (size_t i = 0; i < table->msize; i++) {
        free_ks(table->ks + i); 
    }

    free(table->ks);
    free(table);
}

static char same_keys(const KeySpace * const ks, const char * const key) {
    return (ks->busy == BUSY && cmp(ks->key, key) == 0);
}

static char comparing_same_ks(const KeySpace * const first_ks, const char * const key, size_t * const release) {
    if (!first_ks || !key) {
        return 0;
    }
    
    if (same_keys(first_ks, key)) {
        if (first_ks->release > *release) {
            *release = first_ks->release;
        }
    }
    
    return 1;
}

static size_t find_max_release(const Table * const table, const char * const key) {
    size_t release = 0;
    size_t h1 = hash1(key, table->msize);
    size_t h2 = hash2(key);
    size_t pos = h1;

    for (size_t i = 0; i < table->msize; i++) {
        comparing_same_ks(table->ks + pos, key, &release);
        pos = (pos + h2) % table->msize;
    }
    return release;
}

static void set_ks(KeySpace * const ks, const size_t busy, const char * const key, const size_t info, const size_t release) {
    if (!ks || !key || !info) {
        return;
    }
    
    if (ks->key) {
        free(ks->key);
        ks->key = NULL;
    }
    if (ks->info) {
        info_free(ks->info);
        ks->info = NULL;
    }

    ks->busy = busy;
    ks->key = strdup(key);
    if (release != 0) {
        ks->release = release;
    }
    
    ks->info = info_create();
    if (!ks->info) {
        free(ks->key);
        ks->key = NULL;
        ks->busy = EMPTY;
        return;    
    }

    if (!info_insert(ks->info, info)) {
        free(ks->key);
        info_free(ks->info);
        ks->key = NULL;
        ks->info = NULL;
        ks->busy = EMPTY;
    }

    return;
}

DynamicArray* find_elements(const Table * const table, const char * const key) {
    DynamicArray *da = da_create(sizeof(KeySpace*));
    if (!da) {
        return NULL;
    }

    size_t h1 = hash1(key, table->msize);
    size_t h2 = hash2(key);
    size_t pos = h1;

    for (size_t i = 0; i < table->msize; i++) {
        if (same_keys(table->ks + pos, key)) {
            if (!da_append(da, &(table->ks[pos]))) {
                da_free(da);
                return NULL;
            }
        }
        else if (table->ks[pos].busy == EMPTY) {
            break;
        }
        pos = (pos + h2) % table->msize;
    }

    return da;
}

table_err insert_key_to_table(Table * const table, const char * const key, const size_t info) {
    if (!table || !key) {
        return TABLE_NULL;
    }
    if (table->csize >= table->msize) {
        return TABLE_FULL;
    }

    size_t h1 = hash1(key, table->msize);
    size_t h2 = hash2(key);
    size_t pos = h1;

    size_t release = find_max_release(table, key) + 1;
    for (size_t i = 0; i < table->msize; i++) {
        if (table->ks[pos].busy != BUSY) {
            table->ks[pos].info = info_create();
            if (!table->ks[pos].info) {
                return TABLE_MEM;
            }
            set_ks(table->ks + pos, BUSY, key, info, release);
            if (table->ks[pos].busy != BUSY) {
                return TABLE_MEM;
            }
            table->csize++;
            printf("\nhash1, hash2, pos: %zu %zu %zu\n", h1, h2, pos);
            return TABLE_OK;
        }
        printf("\nhash1, hash2, pos: %zu %zu %zu\n", h1, h2, pos);
        pos = (pos + h2) % table->msize;
    }

    return TABLE_FULL;
}

table_err delete_key_from_table(Table * const table, const char * const key) {
    if (!table) {
        return TABLE_NULL;
    }
    if (!key) {
        return TABLE_VAL;
    }
    if (table->csize == 0) {
        return TABLE_EMPTY;
    }

    size_t h1 = hash1(key, table->msize);
    size_t h2 = hash2(key);
    size_t pos = h1;
    size_t found = 0;

    for (size_t i = 0; i < table->msize; i++) {
        if (same_keys(table->ks + pos, key)) {
            table->ks[pos].busy = DELETED;
            table->csize--;
            found = 1;
        }
        else if (table->ks[pos].busy == EMPTY) {
            break;
        }
        pos = (pos + h2) % table->msize;
    }

    return found ? TABLE_OK : TABLE_VAL;
}

KeySpace* search_by_key_with_release_in_table(const Table * const table, const char * const key, const size_t release) {
    if (!table || !key) {
        return NULL;
    }

    size_t h1 = hash1(key, table->msize);
    size_t h2 = hash2(key);
    size_t pos = h1;

    for (size_t i = 0; i < table->msize; i++) {
        if (same_keys(table->ks + pos, key) && table->ks[pos].release == release) {

            KeySpace *copy = (KeySpace*)calloc(1, sizeof(KeySpace));
            copy->info = info_create();
            if (!copy) {
                return NULL;
            }
            set_ks(copy, BUSY, key, table->ks[pos].info->info, table->ks[pos].release);

            if (!copy->key || !copy->info) {
                free(copy->key);
                free(copy->info);
                free(copy);
                return NULL;
            }

            return copy;
        }
        else if (table->ks[pos].busy == EMPTY) {
            break;
        }
        pos = (pos + h2) % table->msize;
    }
    return NULL;
}

KeySpace** search_by_key_in_table(const Table * const table, const char * const key, size_t *count) {
    if (!table || !key || !count) {
        return NULL;
    }

    DynamicArray *da = da_create(sizeof(KeySpace**));
    if (!da) {
        return NULL;
    }

    size_t h1 = hash1(key, table->msize);
    size_t h2 = hash2(key);
    size_t pos = h1;
    
    for (size_t i = 0; i < table->msize; i++) {
        if (same_keys(table->ks + pos, key)) {
            KeySpace *copy = (KeySpace*)calloc(1, sizeof(KeySpace));
            if (!copy) {
                da_free(da);
                info_free(copy->info);
                free(copy);
                da_free(da);
                return NULL;
            }

            set_ks(copy, BUSY, key, table->ks[pos].info->info, table->ks[pos].release);
            if (!copy->info || !copy->key) {
                free(copy->key);
                info_free(copy->info);
                free(copy);
                da_free(da);
                return NULL;
            }

            if (!da_append(da, &copy)) {
                free(copy->key);
                info_free(copy->info);
                free(copy);
                da_free(da);
                return NULL;
            }
        }
        else if (table->ks[pos].busy == EMPTY) {
            break;
        }
        pos = (pos + h2) % table->msize;
    }
    
    if(da->count == 0) {
        da_free(da);
        return NULL;
    }

    *count = da->count;
    KeySpace **result = (KeySpace**)da->array;
    free(da);
    return result;
}

table_err print_table(const Table * const table) {
    if (!table) {
        return TABLE_NULL;
    }

    printf("\nTable contents (%zu/%zu):\n", table->csize, table->msize);
    printf("┌───────┬──────────┬────────────────────────────────┬──────────────────────┬───────────┐\n");
    printf("│ Index │ Status   │ Key                            │ Info                 │ Release   │\n");
    printf("├───────┼──────────┼────────────────────────────────┼──────────────────────┼───────────┤\n");
    for (size_t i = 0; i < table->msize; i++) {
        char *status;
        switch(table->ks[i].busy) {
            case EMPTY:  status = "EMPTY"; break;
            case BUSY:   status = "BUSY"; break;
            case DELETED: status = "DELETED"; break;
            default:     status = "UNKNOWN"; break;
        }
        print_ks(table->ks + i, status, i);   
        if (i < table->msize - 1) {
            printf("├───────┼──────────┼────────────────────────────────┼──────────────────────┼───────────┤\n");
        }
    }
    printf("└───────┴──────────┴────────────────────────────────┴──────────────────────┴───────────┘\n");

    return TABLE_OK;
}

static table_err check_magic_word(FILE * const file) {
    char magic_word[sizeof(MAGIC_WORD)] = {0};

    if (fread(magic_word, sizeof(char), sizeof(MAGIC_WORD) - 1, file) != sizeof(MAGIC_WORD) - 1 || cmp(magic_word, MAGIC_WORD) != 0) {
        return TABLE_MAGIC_WORD;
    }

    return TABLE_OK;
}

static table_err check_table_size(size_t size) {
    return (size < 1) ? TABLE_SIZE : TABLE_OK; 
}

static table_err read_offsets(long * key_offsets, long * info_offsets, FILE * const file, const size_t len) {
    if (fread(key_offsets, sizeof(long), len, file) != len || fread(info_offsets, sizeof(long), len, file) != len) {
        free(key_offsets);
        free(info_offsets);
        fclose(file);
        return TABLE_VAL;
    }
    return TABLE_OK;
}

static table_err read_number_from_binary(FILE * const file, size_t * const number) {
    if (fread(number, sizeof(size_t), 1, file) != 1) {
        return TABLE_VAL;
    }
    return TABLE_OK;
}

static char *read_key(FILE * const file) {
    size_t key_len;
    if (read_number_from_binary(file, &key_len) != TABLE_OK) {
        return NULL;
    }

    char *key = (char*)calloc(key_len + 1, sizeof(char));
    if (!key) {
        return NULL;
    }

    if (fread(key, sizeof(char), key_len, file) != key_len) {
        free(key);
        return NULL;
    }

    key[key_len] = '\0';
    return key;
}

static table_err read_ks(Table * const table, FILE * const file, const long key_offset, const long info_offset) {
    size_t release;
    if (read_number_from_binary(file, &release) != TABLE_OK){
        return TABLE_VAL;
    }

    fseek(file, key_offset, SEEK_SET);

    char *key = read_key(file);
    if (!key) {
        return TABLE_MEM;
    }

    fseek(file, info_offset, SEEK_SET);

    size_t info;
    if (read_number_from_binary(file, &info) != TABLE_OK) {
        free(key);
        return TABLE_VAL;
    }

    table_err err = insert_key_to_table(table, key, info);
    if (err != TABLE_OK) {
        free(key);
        return err;
    }

    free(key);
    return TABLE_OK;
}

table_err import_table_from_file(Table * const table, const char * const filename) {
    if (table->csize == table->msize) {
        return TABLE_FULL;
    }

    if (!table) {
        return TABLE_NULL;
    }

    if (!filename) {
        return FILE_ERR;
    }
    
    table_err result = TABLE_OK;
    FILE *file = fopen(filename, "r+b");
    if (!file) {
        return FILE_ERR;
    }

    if (check_magic_word(file) != TABLE_OK) {
        result = TABLE_MAGIC_WORD;
        goto exit_with_err;
    }

    size_t size_from_file;
    if (read_number_from_binary(file, &size_from_file) != TABLE_OK) {
        result = TABLE_VAL;
        goto exit_with_err;
    }
    
    if (check_table_size(size_from_file) != TABLE_OK) {
        result = TABLE_SIZE;
        goto exit_with_err;
    }
    
    long *key_offsets = (long*)calloc(size_from_file, sizeof(long));
    long *info_offsets =  (long*)calloc(size_from_file, sizeof(long));
    if (!key_offsets || !info_offsets) {
        free(key_offsets);
        free(info_offsets);
        result = TABLE_MEM;
        goto exit_with_err;
    }

    if (read_offsets(key_offsets, info_offsets, file, size_from_file) != TABLE_OK) {
        free(key_offsets);
        free(info_offsets);
        result = TABLE_VAL;
        goto exit_with_err;
    }
    
    size_t i = 0;
    while (table->csize < table->msize && size_from_file > 0) {
        result = read_ks(table, file, key_offsets[i], info_offsets[i]);
        size_from_file--;
        i++;
    }
    
    free(key_offsets);
    free(info_offsets);
    goto exit_with_err;

exit_with_err:
    fclose(file);
    return result;
}

static table_err write_magic_word(FILE * const file) {
    if (fwrite(MAGIC_WORD, sizeof(char), sizeof(MAGIC_WORD) - 1, file) != sizeof(MAGIC_WORD) - 1) {
        return FILE_ERR;
    }
    return TABLE_OK;
}

static table_err write_table_size(FILE * const file, const size_t * const size) {
    if (fwrite(size, sizeof(size_t), 1, file) != 1) {
        return FILE_ERR;
    }
    return TABLE_OK;
}

static table_err write_key(FILE * const file, const char * const key, const size_t * const key_len) {
    if (fwrite(key_len, sizeof(size_t), 1, file) != 1 || fwrite(key, sizeof(char), *key_len, file) != *key_len) {
        return FILE_ERR;
    }
    return TABLE_OK;
}

static table_err write_info(FILE * const file, const size_t * const info) {
    if (fwrite(info, sizeof(size_t), 1, file) != 1) {
        return FILE_ERR;
    }
    return TABLE_OK;
}

static table_err write_ks(const Table * const table, FILE * const file, size_t i, long * const key_offsets, long * const info_offsets) {
    const KeySpace *ks = &table->ks[i];
    if (ks->busy != BUSY) {
        return TABLE_OK;
    }

    if (fwrite(&ks->release, sizeof(size_t), 1, file) != 1) {
        return FILE_ERR;
    }

    key_offsets[i] = ftell(file);
    size_t key_len = strlen(ks->key);
    if (write_key(file, ks->key, &key_len) != TABLE_OK) {
        return FILE_ERR;
    }

    info_offsets[i] = ftell(file);
    if (write_info(file, &ks->info->info) != TABLE_OK) {
        return FILE_ERR;
    }

    return TABLE_OK;
}

table_err export_table_to_file(const Table * const table, const char * const filename) {
    if (table->csize == 0) {
        return TABLE_EMPTY;
    }

    if (!table) {
        return TABLE_NULL;
    }
    if (!filename) {
        return FILE_ERR;
    }

    table_err result = TABLE_OK;
    FILE* file = fopen(filename, "wb");
    if (!file) {
        return FILE_ERR;
    }

    if (write_magic_word(file) != TABLE_OK) {
        result = FILE_ERR;
        goto exit_err;
    }

    if (write_table_size(file, &(table->csize)) != TABLE_OK) {
        result = FILE_ERR;
        goto exit_err;
    }

    long offsets_pos = ftell(file);
    
    long *key_offsets = calloc(table->csize, sizeof(long));
    long *info_offsets = calloc(table->csize, sizeof(long));
    if (!key_offsets || !info_offsets) {
        free(key_offsets);
        free(info_offsets);
        result = TABLE_MEM;
        goto exit_err;
    }

    fseek(file, offsets_pos + 2 * table->csize * sizeof(long), SEEK_SET);

    for (size_t i = 0; i < table->msize; i++) {
        if (table->ks[i].busy != BUSY) {
            continue;
        }

        result = write_ks(table, file, i, key_offsets, info_offsets);
    }

    fseek(file, offsets_pos, SEEK_SET);
    if (fwrite(key_offsets, sizeof(size_t), table->csize, file) != table->csize || fwrite(info_offsets, sizeof(size_t), table->csize, file) != table->csize) {
        free(key_offsets);
        free(info_offsets);
        result = FILE_ERR;
        goto exit_err;
    }

    free(key_offsets);
    free(info_offsets);
    goto exit_err;

exit_err:
    fclose(file);
    return result;

}
