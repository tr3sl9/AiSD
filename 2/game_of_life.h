#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

typedef struct Generation;

typedef enum cell_state {
	live = '#',
	die = ' '
} cell_state;

char **create_field(size_t, size_t);
void free_field(char**, size_t);
Generation *create_gen(size_t, size_t);
void free_gen(Generation*);
int count_neighbout(Generation*, size_t, size_t);
cell_state change_state(char*, size_t);
Generation *next_gen(Generation*);
Generation *load_initial_state(const char*);
#endif
