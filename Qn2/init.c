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
            proc_attr_table[1] = _gen_sjf_burst_time(proc_attr_table[0]);
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
            proc_attr_table[1] = _gen_rr_burst_time(rr_quantum);
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
    return _gen_rand_table(ARRIVAL_START, ARRIVAL_END, NUM_PROC, NUM_PROC);
}

int* _gen_burst_time() {
    return _gen_rand_table(BURST_START, BURST_END, NUM_PROC, NUM_PROC);
}

int* _gen_priority() {
    return _gen_rand_table(PRIO_START, PRIO_END, NUM_PROC, NUM_PROC);
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
    int start = ARRIVAL_START, end = ARRIVAL_END, num_processes = NUM_PROC, limit = NUM_PROC;
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
            start = ARRIVAL_START;
            goto regen_table;
        }
        // Generate random number based on burst time
        rand_table[index] = rand() % (end - start + 1) + start;
        if (burst_time_obj[i].value == start)
            start = rand_table[index];
        else
            start = rand_table[index] + 1;
    }
    start = ARRIVAL_START;

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

int* _gen_sjf_burst_time(int* arrival_time) {
    int start = BURST_START, end = BURST_END, num_processes = NUM_PROC, limit = NUM_PROC;
    // Save arrival times in struct object
    struct time arrival_time_obj[num_processes];
    for (int i=0; i<num_processes; i++) {
        arrival_time_obj[i].value = arrival_time[i];
        arrival_time_obj[i].index = i;
    }
    // Sort arrival times to retrieve indexes
    qsort(arrival_time_obj, num_processes, sizeof(arrival_time_obj[0]), cmp);

    // Allocate memory
    int* rand_table = (int *)malloc(num_processes * sizeof(int));

    int prev_burst_time = 0;
regen_table:
    for (int i=num_processes-1; i>=0; i--) {
        int index = arrival_time_obj[i].index;
        if (num_processes - i > 1) {
            // Calculate remaining time using previous generated burst time minus current arrival time
            int remaining_time = prev_burst_time - arrival_time_obj[i].value;
            if (remaining_time >= 0) {
                start = start > remaining_time ? start : remaining_time;
            } else {
                start = BURST_START;
            }
        }
        // Generate random number based on arrival time
        rand_table[index] = rand() % (end - start + 1) + start;
        prev_burst_time = rand_table[index];
    }
    start = BURST_START;

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

int* _gen_rr_burst_time(int rr_quantum) {
    int start = BURST_START, end = BURST_END, num_processes = NUM_PROC, limit = NUM_PROC;
    // Set start of random time as the larger value
    start = rr_quantum > start ? rr_quantum : start;

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

