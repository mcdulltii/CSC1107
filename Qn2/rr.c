#include <malloc.h>
#include "init.h"
#include "attr.h"
#include "gantt.h"

float* rr_scheduling(struct process* proc_table, int q_val) {
    float* proc_sch_table = (float *)malloc(3 * sizeof(float));
    int proc_sch_index = 0;
    int* proc_sch_order = (int *)malloc(NUM_PROC * 5 * sizeof(int));
    int* proc_sch_time = (int *)malloc(NUM_PROC * 5 * sizeof(int));

#pragma region PROC_SCH_ALGO
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    int total_response_time = 0;
    float throughput;

    int current_time, prev_current_time = 0;
    int completed = 0;

    // keep track of remaining burst time for each process
    int* remaining_time = (int *)malloc(NUM_PROC * sizeof(int));
    for (int i = 0; i < NUM_PROC; i++) {
        remaining_time[i] = proc_table[i].burst_time;
    }

    int selected_process, prev_process = -1;
    // algorithm starts here
    while (completed != NUM_PROC) {
        selected_process = -1;

        // Find the process with the lowest arrival time that has arrived and not completed yet
        for (int i = 0; i < NUM_PROC; i++) {
            if (proc_table[i].arrival_time <= current_time && \
                    proc_table[i].remaining_time > 0) {
                // Select the process with the lowest arrival time
                if (selected_process == -1 || \
                        proc_table[i].arrival_time < proc_table[selected_process].arrival_time) {
                    selected_process = i;
                } else if (selected_process != -1 && \
                        proc_table[i].arrival_time == proc_table[selected_process].arrival_time) {
                    if (proc_table[i].arrival_time == proc_table[selected_process].arrival_time && \
                            proc_table[i].pid < proc_table[selected_process].pid) {
                        // Select the process that has lower pid if processes have same arrival time
                        selected_process = i;
                    }
                }
            }
        }

        // No processes selected, step forward time unit
        if (selected_process == -1) {
            current_time++;
            continue;
        }

        // Execute the selected process if remaining time lower than quantum value
        if (proc_table[selected_process].remaining_time <= q_val)
        {
            current_time += proc_table[selected_process].remaining_time;
            proc_table[selected_process].remaining_time = 0;
        }

        else {
            // Execute the selected process for unit of quantum value
            proc_table[selected_process].remaining_time = proc_table[selected_process].remaining_time-q_val;
            current_time++;
        }
        

        // Save order of process schedule and start and end time
        if (prev_process != selected_process) {
            proc_sch_order[proc_sch_index] = proc_table[selected_process].pid;
            proc_sch_time[proc_sch_index * 2] = prev_current_time;
            proc_sch_time[proc_sch_index * 2 + 1] = current_time;
            proc_sch_index++;
            prev_current_time = current_time;
        }

        // Update start time and end time for the process if it just started or completed
        if (proc_table[selected_process].start_time == -1) {
            proc_table[selected_process].start_time = current_time - 1;
        }
        if (proc_table[selected_process].remaining_time == 0) {
            proc_table[selected_process].end_time = current_time;
            completed++;
        }
        prev_process = selected_process;
        
    }

#pragma endregion PROC_SCH_ALGO

    for (int i = 0; i < NUM_PROC; i++) {
        proc_table[i].turnaround_time = proc_table[i].end_time - proc_table[i].arrival_time;
        proc_table[i].waiting_time = proc_table[i].turnaround_time - proc_table[i].burst_time;
        proc_table[i].response_time = proc_table[i].start_time - proc_table[i].arrival_time;
        total_turnaround_time += proc_table[i].turnaround_time;
        total_waiting_time += proc_table[i].waiting_time;
        total_response_time += proc_table[i].response_time;
    }

    proc_sch_table[0] = total_turnaround_time / NUM_PROC;
    proc_sch_table[1] = total_waiting_time / NUM_PROC;
    proc_sch_table[2] = total_response_time / NUM_PROC;

    // Draw gantt chart
    visualise_gantt(proc_sch_order, proc_sch_time, proc_sch_index);
    free(proc_sch_order);
    free(proc_sch_time);
    return proc_sch_table;
}
