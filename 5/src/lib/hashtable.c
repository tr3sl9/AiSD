#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

#include "hashtable.h"
#include "libgraph.h"

#define HASH_SEED 0x5bd1e995
#define MAGIC_CONST 0xc6a4a7935bd1e995ULL
#define MAGIC_WORD "HASH_TABLE\n"

// usual hash
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

static int cmp(const char * const str_1, const char * const str_2) {
    return strcmp(str_1, str_2);
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

void table_free(Table * const table) {
    if (!table) return;

    for (size_t i = 0; i < table->msize; i++) {
        if (table->ks[i].vertex) {
            free(table->ks[i].vertex->id);
            free(table->ks[i].vertex);
        }
    }

    free(table->ks);
    free(table);

    return;
}

static char same_vertex(const KeySpace * const ks, const char * const id) {
    return (ks->busy == BUSY && ks->vertex && cmp(ks->vertex->id, id) == 0);
}

static void set_ks(KeySpace * const ks, const BusyType busy, Vertex * const vertex) {
    if (!ks || !vertex) {
        return;
    }

    ks->busy = busy;
    ks->vertex = vertex;

    return;
}

static void resize_table(Table * const table) {
    if (!table) {
        return;
    }
    
    size_t old_size = table->msize;
    size_t new_size = old_size * 2;
    
    KeySpace *old_ks = table->ks;
    KeySpace *new_ks = (KeySpace*)calloc(new_size, sizeof(KeySpace));
    if (!new_ks) {
        return;
    }
    
    table->ks = new_ks;
    table->msize = new_size;
    table->csize = 0;
    
    for (size_t i = 0; i < old_size; i++) {
        if (old_ks[i].busy == BUSY && old_ks[i].vertex) {
            size_t h1 = hash1(old_ks[i].vertex->id, new_size);
            size_t h2 = hash2(old_ks[i].vertex->id);
            size_t pos = h1;
            
            for (size_t j = 0; j < new_size; j++) {
                if (new_ks[pos].busy != BUSY) {
                    new_ks[pos].busy = BUSY;
                    new_ks[pos].vertex = old_ks[i].vertex;
                    table->csize++;
                    break;
                }
                pos = (pos + 1 + h2 % (new_size - 1)) % new_size;
            }
        }
    }
    
    free(old_ks);

    return;
}

table_err insert_vertex_table(Table * const table, Vertex * const vertex) {
    if (!table || !vertex) {
        return TABLE_NULL;
    }
    
    KeySpace *existing = search_vertex_in_table(table, vertex->id);
    if (existing) {
        return TABLE_VAL;
    }
    
    if (table->csize >= table->msize / 2) {
        resize_table(table);
    }

    size_t h1 = hash1(vertex->id, table->msize);
    size_t h2 = hash2(vertex->id);
    size_t pos = h1;

    for (size_t i = 0; i < table->msize; i++) {
        if (table->ks[pos].busy != BUSY) {
            set_ks(table->ks + pos, BUSY, vertex);
            table->csize++;
            return TABLE_OK;
        }

        pos = (pos + 1 + h2 % (table->msize - 1)) % table->msize;
    }

    return TABLE_FULL;
}

table_err delete_vertex_table(Table * const table, const char * const id) {
    if (!table) {
        return TABLE_NULL;
    }
    if (!id) {
        return TABLE_VAL;
    }
    if (table->csize == 0) {
        return TABLE_EMPTY;
    }

    size_t h1 = hash1(id, table->msize);
    size_t h2 = hash2(id);
    size_t pos = h1;
    size_t found = 0;

    for (size_t i = 0; i < table->msize; i++) {
        if (same_vertex(table->ks + pos, id)) {
            table->ks[pos].busy = DELETED;
            table->csize--;
            found = 1;
            break;
        }
        else if (table->ks[pos].busy == EMPTY) {
            break;
        }

        pos = (pos + 1 + h2 % (table->msize - 1)) % table->msize;
    }

    return found ? TABLE_OK : TABLE_VAL;
}

KeySpace* search_vertex_in_table(const Table * const table, const char * const id) {
    if (!table || !id) {
        return NULL;
    }

    size_t h1 = hash1(id, table->msize);
    size_t h2 = hash2(id);
    size_t pos = h1;

    for (size_t i = 0; i < table->msize; i++) {
        if (same_vertex(table->ks + pos, id)) {
            return table->ks + pos;
        }
        else if (table->ks[pos].busy == EMPTY) {
            break;
        }

        pos = (pos + 1 + h2 % (table->msize - 1)) % table->msize;
    }

    return NULL;
}
