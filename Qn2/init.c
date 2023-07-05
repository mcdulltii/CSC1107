#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <malloc.h>
#include "init.h"

// Sort compare function
int cmp(const void *a, const void *b)
{
    struct time *a1 = (struct time *)a;
    struct time *a2 = (struct time *)b;
    if ((*a1).value > (*a2).value)
        return -1;
    else if ((*a1).value < (*a2).value)
        return 1;
    else
        return 0;
}

// Custom print function
void print(const char* output_str, char* filename) {
    // Prints output_str and logs to output filename
    printf("%s", output_str);
    FILE* fptr = fopen(filename, "a");
    fprintf(fptr, "%s", output_str);
    fclose(fptr);
}

int** init(int scheduling_selection, int rr_quantum, bool is_preempt) {
    // Set random seed
    srand(time(NULL));

    // Return process attributes as [arrival_time, burst_time, priority]
    int** proc_attr_table = (int**)malloc(3 * sizeof(int*));
    switch (scheduling_selection) {
        case 0:
            // FCFS Scheduling
            proc_attr_table[1] = _gen_burst_time();
            proc_attr_table[0] = _gen_fcfs_arrival_time(proc_attr_table[1]);
            proc_attr_table[2] = _gen_priority();
            break;
        case 1:
            // SJF Scheduling
            proc_attr_table[0] = _gen_arrival_time();
            proc_attr_table[1] = _gen_burst_time();
            proc_attr_table[2] = _gen_priority();
            break;
        case 2:
            // SRTF Scheduling
            proc_attr_table[0] = _gen_arrival_time();
            proc_attr_table[1] = _gen_burst_time();
            proc_attr_table[2] = _gen_priority();
            break;
        case 3:
            // RR Scheduling
            proc_attr_table[0] = _gen_arrival_time();
            proc_attr_table[1] = _gen_burst_time();
            proc_attr_table[2] = _gen_priority();
            break;
        case 4:
            // Priority Scheduling
            proc_attr_table[0] = _gen_arrival_time();
            proc_attr_table[1] = _gen_burst_time();
            proc_attr_table[2] = _gen_priority();
            break;
        default:
            printf("Invalid selection!\n");
            exit(-1);
    }

    return proc_attr_table;
}

int** init_usr() {
    // Return process attributes as [arrival_time, burst_time, priority]
    int** proc_attr_table = (int**)malloc(3 * sizeof(int*));
    proc_attr_table[0] = _get_attr_values("arrival time");
    proc_attr_table[1] = _get_attr_values("burst time");
    proc_attr_table[2] = _get_attr_values("priority");
    return proc_attr_table;
}

int* _gen_rand_table(int start, int end, int num_processes, int limit) {
    // Allocate memory
    int* rand_table = (int *)malloc(num_processes * sizeof(int));

regen_table:
    for (int i=0; i<num_processes; i++) {
        // Generate random number from `start` to `end`
        rand_table[i] = rand() % (end - start + 1) + start;
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
    return _gen_rand_table(0, 8, NUM_PROC, NUM_PROC);
}

int* _gen_burst_time() {
    return _gen_rand_table(3, 10, NUM_PROC, NUM_PROC);
}

int* _gen_priority() {
    return _gen_rand_table(1, 4, NUM_PROC, NUM_PROC);
}

int* _get_attr_values(const char* attr_name) {
    int* attr_table = (int *)malloc(NUM_PROC * sizeof(int));
    printf("Enter the %s for each process:\n", attr_name);
    for (int i = 0; i < NUM_PROC; i++) {
        printf("Process %d: ", i + 1);
        scanf("%d%*c", &attr_table[i]);
    }
    return attr_table;
}

int* _gen_fcfs_arrival_time(int* burst_time) {
    int start = 0, end = 8, num_processes = NUM_PROC, limit = NUM_PROC;
    // Save burst times in struct object
    struct time burst_time_obj[num_processes];
    for (int i=0; i<num_processes; i++) {
        burst_time_obj[i].value = burst_time[i];
        burst_time_obj[i].index = i;
    }
    // Sort burst times to retrieve indexes
    qsort(burst_time_obj, num_processes, sizeof(burst_time_obj[0]), cmp);

    // Allocate memory
    int* rand_table = (int *)malloc(num_processes * sizeof(int));

regen_table:
    for (int i=num_processes-1; i>=0; i--) {
        int index = burst_time_obj[i].index;
        if (start > end) {
            start = 0;
            goto regen_table;
        }
        // Generate random number based on burst time
        rand_table[index] = rand() % (end - start + 1) + start;
        if (burst_time_obj[i].value == start)
            start = rand_table[index];
        else
            start = rand_table[index] + 1;
    }
    start = 0;

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

