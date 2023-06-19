#include <malloc.h>
#include "init.h"
#include "attr.h"

float* srtf_scheduling(struct process* proc_table) {
#pragma region PROC_SCH_ALGO
    // Variable initializations
    int current_time = 0;
    int completed = 0;
    for (int i = 0; i < NUM_PROC; i++) {
        proc_table[i].remaining_time = proc_table[i].burst_time;
    }

    // Process scheduling loop
    while (completed != NUM_PROC) {
        int selected_process = -1;

        // Find the process with the shortest remaining time that has arrived and not completed yet
        for (int i = 0; i < NUM_PROC; i++) {
            if (proc_table[i].arrival_time <= current_time && \
                    proc_table[i].remaining_time > 0) {
                // Select the process with the shortest remaining time
                if (selected_process == -1 || \
                        proc_table[i].remaining_time < proc_table[selected_process].remaining_time) {
                    selected_process = i;
                } else if (selected_process != -1 && \
                        proc_table[i].remaining_time == proc_table[selected_process].remaining_time) {
                    // Select the process that arrives first if processes have the same remaining time
                    if (proc_table[i].arrival_time < proc_table[selected_process].arrival_time) {
                        selected_process = i;
                    } else if (proc_table[i].arrival_time == proc_table[selected_process].arrival_time && \
                            proc_table[i].pid < proc_table[selected_process].pid) {
                        // Select the process that has a lower pid if processes have the same remaining time and arrival time
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

        // Execute the selected process for one unit of time
        proc_table[selected_process].remaining_time--;
        current_time++;

        // Update start time and end time for the process if it just started or completed
        if (proc_table[selected_process].start_time == -1) {
            proc_table[selected_process].start_time = current_time - 1;
        }
        if (proc_table[selected_process].remaining_time == 0) {
            proc_table[selected_process].end_time = current_time;
            completed++;
        }
    }
#pragma endregion PROC_SCH_ALGO

#pragma region PROC_SCH_CALC
    float* proc_sch_table = (float*)malloc(3 * sizeof(float));

    // Calculate turnaround time, waiting time, and response time
    float total_turnaround_time = 0;
    float total_waiting_time = 0;
    float total_response_time = 0;
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
#pragma endregion PROC_SCH_CALC

    return proc_sch_table;
}
