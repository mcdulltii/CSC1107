#include <time.h>
#include <stdlib.h>
#include <malloc.h>

#define NUM_PROC 6

void gen_attr();
int* gen_rand_table(int, int, int, int);
int* gen_arrival_time();
int* gen_burst_time();
int* gen_priority();

void gen_attr() {
    // Set random seed
    srand(time(NULL));
}

int* gen_rand_table(int start, int end, int num_processes, int limit) {
    // Allocate memory
    int* rand_table = (int *)malloc(num_processes * sizeof(int));
    int* dup_table = (int *)calloc(end - start, sizeof(int));

regen_table:
    for (int i=0; i<num_processes; i++) {
        // Generate random number from `start` to `end`
        rand_table[i] = rand() % end + start;
    }

    // Check for duplicates
    for (int i=0; i<num_processes; i++) {
        dup_table[rand_table[i] - start]++;
        if (dup_table[rand_table[i] - start] > limit) {
            // Regenerate random values since duplicate limit reached
            goto regen_table;
        }
    }
    free(dup_table);
    return rand_table;
}

int* gen_arrival_time() {
    return gen_rand_table(0, 8, NUM_PROC, 3);
}


int* gen_burst_time() {
    return gen_rand_table(3, 10, NUM_PROC, 3);
}

int* gen_priority() {
    return gen_rand_table(1, 4, NUM_PROC, 2);
}

