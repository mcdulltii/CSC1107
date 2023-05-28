#include <stdio.h>
#include <stdlib.h>
#include "attr.h"

int main(int argc, char *argv[]) {
    int* arrival_time = gen_arrival_time();
    int* burst_time = gen_burst_time();
    int* priority = gen_burst_time();

    for (int i=0; i<NUM_PROC; i++) {
        printf("Process %d: %d %d %d\n", i, arrival_time[i], burst_time[i], priority[i]);
    }

    free(arrival_time);
    free(burst_time);
    free(priority);
}
