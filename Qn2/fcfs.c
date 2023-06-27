#include <malloc.h>
#include "init.h"
#include "attr.h"
#include "gantt.h"

float* fcfs_scheduling(struct process* proc_table) {
    // float* proc_sch_table = (float *)malloc(3 * sizeof(float));
    int proc_sch_index = 0;
    int* proc_sch_order = (int *)malloc(NUM_PROC * 5 * sizeof(int));
    int* proc_sch_time = (int *)malloc(NUM_PROC * 5 * sizeof(int));

#pragma region PROC_SCH_ALGO
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    int total_response_time = 0;

    int current_time = 0;
    int completed = 0;
    int prev_current_time = 0;

    // Sort the processes based on arrival time using bubble sort
    for (int i = 0; i < NUM_PROC - 1; i++) {
        for (int j = i + 1; j < NUM_PROC; j++) {
            if (proc_table[j].arrival_time < proc_table[i].arrival_time) {
                struct process temp = proc_table[i];
                proc_table[i] = proc_table[j];
                proc_table[j] = temp;
            }
        }
    }

    // Process scheduling loop
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

        // Execute the selected process for one unit of time
        current_time += proc_table[i].remaining_time;
        proc_table[i].remaining_time = 0;

        proc_sch_order[proc_sch_index] = proc_table[i].pid;
        proc_sch_time[proc_sch_index * 2] = prev_current_time;
        proc_sch_time[proc_sch_index * 2 + 1] = current_time;
        proc_sch_index++;
        prev_current_time = current_time;

        // Update start time and end time for the process if it just started or completed
        if (proc_table[i].start_time == -1) {
            proc_table[i].start_time = current_time - 1;
        }
        if (proc_table[i].remaining_time == 0) {
            proc_table[i].end_time = current_time;
            completed++;
        }
    }
    proc_sch_time[proc_sch_index * 2] = -1;
#pragma endregion 

#pragma region PROC_SCH_CALC
    float* proc_sch_table = (float*)malloc(3 * sizeof(float));

    proc_sch_table[0] = total_turnaround_time / NUM_PROC;
    proc_sch_table[1] = total_waiting_time / NUM_PROC;
    proc_sch_table[2] = total_response_time / NUM_PROC;

    // Draw gantt chart
    visualise_gantt(proc_sch_order, proc_sch_time, proc_sch_index);
    free(proc_sch_order);
    free(proc_sch_time);
#pragma endregion PROC_SCH_CALC
    
    return proc_sch_table;
}
