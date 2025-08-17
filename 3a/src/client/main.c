#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/table.h"
#include "dialog.h"
#include "testing.h"

#define PROMPT_FOR_CHOICE_COMMAND "Choice command: "

int main(int argc, char **argv){
    if (argc != 2) {
        printf("Error: not enough argumetns\n");
        return 1;
    }
    int atoi_argv_1 = atoi(argv[1]);
    if (atoi_argv_1 <= 0) {
        printf("Error: table size mismatch\n");
        return 1;
    }
    Table *table = create_table(atoi_argv_1);
    int end_program = 0;
    while (end_program != 1){
        show_menu();
        size_t choice;
        if(read_number(&choice, 0, 9, PROMPT_FOR_CHOICE_COMMAND) == TABLE_EOF) {
            free_table(table);
            return 1;
        }
        end_program = process_choice(table, choice);
    }
    free_table(table);
    return 0;
}
