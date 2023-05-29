#include <time.h>
#include <stdlib.h>
#include <malloc.h>
#include "attr.h"

int** gen_attr() {
    // Set random seed
    srand(time(NULL));

    // Return process attributes as [arrival_time, burst_time, priority]
    int** proc_attr_table = (int**)malloc(3 * sizeof(int*));
    proc_attr_table[0] = _gen_arrival_time();
    proc_attr_table[1] = _gen_burst_time();
    proc_attr_table[2] = _gen_priority();
    return proc_attr_table;
}

int* _gen_rand_table(int start, int end, int num_processes, int limit) {
    // Allocate memory
    int* rand_table = (int *)malloc(num_processes * sizeof(int));

regen_table:
    for (int i=0; i<num_processes; i++) {
        // Generate random number from `start` to `end`
        rand_table[i] = rand() % end + start;
    }

    // Check for duplicates
    int* dup_table = (int *)calloc(end - start, sizeof(int));
    for (int i=0; i<num_processes; i++) {
        dup_table[rand_table[i] - start]++;
        if (dup_table[rand_table[i] - start] > limit) {
            // Regenerate random values since duplicate limit reached
            free(dup_table);
            goto regen_table;
        }
    }
    free(dup_table);
    return rand_table;
}

int* _gen_arrival_time() {
    return _gen_rand_table(0, 8, NUM_PROC, 3);
}


int* _gen_burst_time() {
    return _gen_rand_table(3, 10, NUM_PROC, 3);
}

int* _gen_priority() {
    return _gen_rand_table(1, 4, NUM_PROC, 2);
}

