#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

typedef struct Generation {
	char **field;
	size_t width;
	size_t height;
} Generation;

typedef enum cell_state {
	live = '#',
	die = '.'
} cell_state;

char **create_field(const size_t, const size_t);
Generation *copy_gen(const Generation*);
void free_field(char**, const size_t);
Generation *create_gen(const size_t, const size_t);
void free_gen(Generation*);
void print_gen(const Generation*);
int count_neighbour(const Generation*, const ssize_t, const ssize_t);
cell_state change_state(const char, const size_t);
Generation *next_gen(const Generation*);
Generation *load_initial_state(const char*);
int save_state_to_file(const char *, const Generation*);
#endif
