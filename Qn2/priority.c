#include <stdbool.h>
#include <malloc.h>
#include "init.h"
#include "attr.h"

float* priority_scheduling(struct process* proc_table, bool preemptive) {
#pragma region PROC_SCH_ALGO
    // Sort the process table based on priority (in ascending order)
    for (int i = 0; i < NUM_PROC - 1; i++) {
        for (int j = 0; j < NUM_PROC - i - 1; j++) {
            if (proc_table[j].priority > proc_table[j + 1].priority) {
                struct process temp = proc_table[j];
                proc_table[j] = proc_table[j + 1];
                proc_table[j + 1] = temp;
            }
        }
    }

    // Variable initializations
    int current_time = 0;
    int completed = 0;
    for (int i = 0; i < NUM_PROC; i++) {
        proc_table[i].remaining_time = proc_table[i].burst_time;
    }

    // Process scheduling loop
    int prev_process = -1;
    while (completed != NUM_PROC) {
        int selected_process = -1;
        int highest_priority = 1000;

        if (preemptive) {
            // Find the process with the highest priority that has arrived and still needs to be executed
            for (int i = 0; i < NUM_PROC; i++) {
                if (proc_table[i].arrival_time <= current_time && \
                        proc_table[i].remaining_time > 0 && \
                        proc_table[i].priority < highest_priority) {
                    // Decide whether to preempt previous process
                    if (proc_table[i].priority == proc_table[prev_process].priority && \
                            proc_table[prev_process].remaining_time > 0) {
                        // Continue previous process without preempting
                        highest_priority = proc_table[prev_process].priority;
                        selected_process = prev_process;
                    } else {
                        // Preempt previous process with new process
                        highest_priority = proc_table[i].priority;
                        selected_process = i;
                    }
                }
            }
        } else {
            // Find the process with the highest priority that has arrived and not completed yet
            for (int i = 0; i < NUM_PROC; i++) {
                if (proc_table[i].arrival_time <= current_time && \
                        proc_table[i].remaining_time > 0) {
                    // Select the process with the highest priority
                    if (selected_process == -1 || \
                            proc_table[i].priority < proc_table[selected_process].priority) {
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

        if (preemptive) {
            // Execute the selected process for one unit of time
            proc_table[selected_process].remaining_time--;
            current_time++;
            // Save previous process
            prev_process = selected_process;
        } else {
            // Execute the selected process for the entire burst time
            current_time += proc_table[selected_process].remaining_time;
            proc_table[selected_process].remaining_time = 0;
        }

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
    float* proc_sch_table = (float *)malloc(3 * sizeof(float));

    // Calculate turnaround time, waiting time, and response time
    float total_turnaround_time = 0;
    float total_waiting_time = 0;
    float total_response_time = 0;
    for (int i=0; i<NUM_PROC; i++) {
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
