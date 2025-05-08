#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "table.h"

#define MAGIC_WORD "HASH_TABLE\n"

//from lektion
static size_t hash1(const char * const key, const size_t msize) {
    size_t hash = SIZE_MAX;
	for (size_t i = 0; key[i] != '\0'; i++) {
		hash = 37 * hash + key[i];
	}
	return hash > 0 ? hash % msize : -(hash) % msize;
}

// MurmurHash
static size_t hash2(const void * const key, size_t len, const size_t seed) {
    const size_t m = 0x5bd1e995;
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
	printf("| %-5zu | %-8s | %-30s | %-20zu | %-9zu |\n", i, status, ks->key, *(ks->info), ks->release);
	return;
}

Table *create_table(const size_t msize) {
    Table *table = (Table*)calloc(1, sizeof(Table));
    if (!table) return NULL;
    table->ks = (KeySpace*)calloc(msize, sizeof(KeySpace));
    if (!table->ks) {
        free(table);
        return NULL;
    }
    table->msize = msize;
    table->csize = 0;
    for (size_t i = 0; i < msize; i++) {
        table->ks[i].busy = EMPTY;
    }
    return table;
}

void free_table(Table * const table) {
    if (!table) return;    
    for (size_t i = 0; i < table->msize; i++) {
        if (table->ks[i].busy == BUSY) {
            free_ks(table->ks + i);        }
    }
    free(table->ks);
    free(table);
}

void free_ks(KeySpace * const ks) {
	if (!ks) {
		return;
	}
	ks->busy = DELETED;
	free(ks->key);
	free(ks->info);
	return;
}

static size_t find_last_release(const Table * const table, const char * const key) {
	size_t release = 0;
	for (size_t i = 0; i < table->csize; i++) {
		if (cmp(table->ks[i].key, key) == 0) {
			release++;
		}
	}
	return release;
}

static void set_ks(KeySpace * const ks, const size_t busy, const char * const key, const size_t info, const size_t release) {
	if (!ks || !key || !info) return;
	ks->busy = busy;
	ks->key = strdup(key);
	if (release != 0) ks->release = release;
	*(ks->info) = info;
	return;
}

table_err insert_key_to_table(Table * const table, const char * const key, const size_t info) {
    if (!table || !key) return TABLE_NULL;
    if (table->csize >= table->msize) return TABLE_FULL;
    size_t h1 = hash1(key, table->msize);
    size_t h2 = hash2(key, strlen(key), 0x9747b28c);
    size_t pos = h1;
    size_t release = find_last_release(table, key) + 1;
    for (size_t i = 0; i < table->msize; i++) {
        if (table->ks[pos].busy != BUSY) {
			set_ks(table->ks + pos, BUSY, key, info, release);
            table->csize++;
            return TABLE_OK;
        }
        pos = (pos + h2) % table->msize;
    }
    return TABLE_FULL;
}

table_err delete_key_from_table(Table * const table, const char * const key) {
    if (!table) return TABLE_NULL;
	if (!key) return TABLE_VAL;
    size_t h1 = hash1(key, table->msize);
    size_t h2 = hash2(key, strlen(key), 0x9747b28c);
    size_t pos = h1;
    size_t found = 0;
    for (size_t i = 0; i < table->msize; i++) {
		if (table->ks[pos].busy == BUSY && cmp(table->ks[pos].key, key) == 0) {
			free_ks(table->ks + pos);
			table->csize--;
			found = 1;
		} else if (table->ks[pos].busy == EMPTY) {
			break;
		}
        pos = (pos + h2) % table->msize;
    }
    return found ? TABLE_OK : TABLE_VAL;
}

KeySpace* search_by_key_with_release_in_table(const Table * const table, const char * const key, const size_t release) {
    if (!table || !key) return NULL;
    size_t h1 = hash1(key, table->msize);
    size_t h2 = hash2(key, strlen(key), 0x9747b28c);
    size_t pos = h1;
	KeySpace *found_key_with_release = (KeySpace*)calloc(1, sizeof(KeySpace));
    for (size_t i = 0; i < table->msize; i++) {
        if (table->ks[pos].busy == BUSY && cmp(table->ks[pos].key, key) == 0 && table->ks[pos].release == release) {
            set_ks(found_key_with_release, BUSY, key, *(table->ks[pos].info), release);
			return found_key_with_release;
        } 
		else if (table->ks[pos].busy == EMPTY) {
            break;
        }
        pos = (pos + h2) % table->msize;
    } 
    return NULL;
}

static size_t count_of_key_in_table(const Table * const table, const char * const key, size_t h1, size_t h2) {
	size_t count = 0;
	size_t pos = h1;
	for (size_t i = 0; i < table->msize; i++) {
		if (table->ks[pos].busy == BUSY && cmp(table->ks[pos].key, key) == 0) {
			count++;
		}
		else if (table->ks[pos].busy == EMPTY) {
			break;
		}
		pos = (pos + h2) % table->msize;
	}
	return count;
}

KeySpace* search_by_key_in_table(const Table * const table, const char * const key) {
    if (!table || !key) return NULL;

    size_t h1 = hash1(key, table->msize);
    size_t h2 = hash2(key, strlen(key), 0x9747b28c);

    size_t count_of_key = count_of_key_in_table(table, key, h1, h2);
    KeySpace* result = (KeySpace*)calloc(count_of_key, sizeof(KeySpace));

    size_t pos = h1;
	size_t capacity = 0;
    for (size_t i = 0; i < table->msize; i++) {
        if (table->ks[pos].busy == BUSY && cmp(table->ks[pos].key, key) == 0) {
			 set_ks(result + capacity, BUSY, key, *(table->ks[pos].info), table->ks[pos].release);
        }
        else if (table->ks[pos].busy == EMPTY) {
            break;
        }
        pos = (pos + h2) % table->msize;
    }
    return result;
}

table_err print_table(const Table * const table) {
    if (!table) return TABLE_NULL;
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

table_err import_table_from_file(Table * const table, const char * const filename) {
	if (!table) return TABLE_NULL;
	if (!filename) return FILE_ERR;
	FILE *file = fopen(filename, "rb");
	if (!file) return FILE_ERR;

	char magic_word[sizeof(MAGIC_WORD)] = {0};
	fread(magic_word, sizeof(char), sizeof(magic_word), file);
	if (cmp(magic_word, MAGIC_WORD) != 0) {
		fclose(file);
		return TABLE_MAGIC_WORD;
	}

	size_t msize, csize;
    if (fread(&msize, sizeof(size_t), 1, file) != 1 || fread(&csize, sizeof(size_t), 1, file) != 1) {
        fclose(file);
        return TABLE_SIZE;
    }
    if (msize != table->msize || csize > msize || msize == 0) {
        fclose(file);
        return TABLE_SIZE;
    }
	for (size_t i = 0; i < csize; i++) {
		size_t key_len, release;
		if (fread(&key_len, sizeof(size_t), 1, file) != 1) {
			fclose(file);
			return TABLE_VAL;
		}	
		char *key = (char*)malloc(key_len + 1);
		if (!key) {
			fclose(file);
			return TABLE_MEM;
		}
		if (fread(key, sizeof(char), key_len, file) != key_len) {
			free(key);
			fclose(file);
			return TABLE_VAL;
		}
		key[key_len] = '\0';
		if (fread(&release, sizeof(size_t), 1, file) != 1) {
			free(key);
			fclose(file);
			return TABLE_VAL;
		}
		size_t info;
		if (fread(&info, sizeof(size_t), 1, file) != 1) {
			free(key);
			fclose(file);
			return TABLE_VAL;
		}
		table_err err = insert_key_to_table(table, key, info);
		if (err != TABLE_OK) {
			free(key);
			fclose(file);
			return err;
		}
		free(key);
		table->csize++;
	}
	fclose(file);
	return TABLE_OK;
}

table_err export_table_to_file(const Table * const table, const char * const filename) {
    if (!table || !filename) return TABLE_NULL;
    FILE* file = fopen(filename, "wb");
    if (!file) return FILE_ERR;
    
    if (fwrite(MAGIC_WORD, sizeof(char), sizeof(MAGIC_WORD) - 1, file) != sizeof(MAGIC_WORD) - 1) {
        fclose(file);
        return FILE_ERR;
    }
    if (fwrite(&table->msize, sizeof(size_t), 1, file) != 1 ||
        fwrite(&table->csize, sizeof(size_t), 1, file) != 1) {
        fclose(file);
        return FILE_ERR;
    }
    for (size_t i = 0; i < table->msize; i++) {
        if (table->ks[i].busy == BUSY) {
            size_t key_len = strlen(table->ks[i].key);
// write all information to file 			
            if (fwrite(&key_len, sizeof(size_t), 1, file) != 1 || fwrite(table->ks[i].key, sizeof(char), key_len, file) != key_len || fwrite(&table->ks[i].release, sizeof(size_t), 1, file) != 1 || fwrite(table->ks[i].info, sizeof(uint32_t), 1, file) != 1) {
                fclose(file);
                return FILE_ERR;
            }
        }
    }
    fclose(file);
    return TABLE_OK;
}
