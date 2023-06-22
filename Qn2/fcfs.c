#include <malloc.h>
#include "init.h"
#include "attr.h"

float* fcfs_scheduling(struct process* proc_table) {
    float* proc_sch_table = (float *)malloc(3 * sizeof(float));

#pragma region PROC_SCH_ALGO
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    int total_response_time = 0;

    int current_time = 0;
    int completed = 0;

    for (int i = 0; i < NUM_PROC; i++) {
        // Wait for process to arrive
        while (current_time < proc_table[i].arrival_time) {
            current_time++;
        }

        // Update waiting time
        total_waiting_time += current_time - proc_table[i].arrival_time;

        // Update response time
        total_response_time += current_time - proc_table[i].arrival_time;

        // Update current time and completion status
        current_time += proc_table[i].burst_time;
        completed++;

        // Update turnaround time
        total_turnaround_time += current_time - proc_table[i].arrival_time;
    }
#pragma endregion PROC_SCH_ALGO

    proc_sch_table[0] = total_turnaround_time / NUM_PROC;
    proc_sch_table[1] = total_waiting_time / NUM_PROC;
    proc_sch_table[2] = total_response_time / NUM_PROC;
    return proc_sch_table;
}
