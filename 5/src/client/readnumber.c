#include <stdio.h>
#include "readnumber.h"
#include "../lib/libgraph.h"

graph_err read_positive_number(size_t * const count, const char * prompt) {
    int p2 = 0;
    while(p2 != 1){
        printf("%s", prompt);
        p2 = scanf("%zu", count);
        if(p2 == 0){
            printf("Error: invalid data type\n");
            printf("Enter the number again: ");
            scanf("%*[^\n]");
        }
        else if(p2 == EOF){
            return GRAPH_EOF;
        }
    }
    return GRAPH_OK;
}

graph_err read_number(int * const count, const int min, const int max, const char * const prompt){
    int p2 = 0;
    while(p2 != 1){
        printf("%s", prompt);
        p2 = scanf("%d", count);
        if(p2 == 0){
            printf("Error: invalid data type\n");
            printf("Enter the number again: ");
            scanf("%*[^\n]");
        }
        else if(p2 == EOF){
            return GRAPH_EOF;
        } else {
            if(*count < min || *count > max){
                p2 = 0;
                printf("The entered number must be greater than %d and less than or equal to %d\n", min, max);
                printf("Enter the number again\n");
                scanf("%*[^\n]");
            }
        }
    }
    return GRAPH_OK;
}

graph_err read_number_double(double * const count, const double min, const double max, const char * const prompt){
    int p2 = 0;
    while(p2 != 1){
        printf("%s", prompt);
        p2 = scanf("%lf", count);
        if(p2 == 0){
            printf("Error: invalid data type\n");
            printf("Enter the number again: ");
            scanf("%*[^\n]");
        }
        else if(p2 == EOF){
            return GRAPH_EOF;
        } else {
            if(*count < min || *count > max){
                p2 = 0;
                printf("The entered number must be greater than %.2f and less than or equal to %.2f\n", min, max);
                printf("Enter the number again\n");
                scanf("%*[^\n]");
            }
        }
    }
    return GRAPH_OK;
}
