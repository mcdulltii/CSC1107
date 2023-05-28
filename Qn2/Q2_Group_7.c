#include <stdio.h>
#include <stdlib.h>
#include "attr.h"

int main(int argc, char *argv[]) {
    int** proc_attr_table = gen_attr();
    int* arrival_time = proc_attr_table[0];
    int* burst_time = proc_attr_table[1];
    int* priority = proc_attr_table[2];

    for (int i=0; i<NUM_PROC; i++) {
        printf("Process %d: %d %d %d\n", i, arrival_time[i], burst_time[i], priority[i]);
    }

    free(arrival_time);
    free(burst_time);
    free(priority);
    free(proc_attr_table);
}
