#include <stdbool.h>
#include <malloc.h>
#include "init.h"
#include "attr.h"
#include "gantt.h"

float* priority_scheduling(struct process* proc_table, bool preemptive) {
    int proc_sch_index = 0;
    int* proc_sch_order = (int *)malloc(NUM_PROC * 5 * sizeof(int));
    int* proc_sch_time = (int *)malloc(NUM_PROC * 5 * sizeof(int));

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
    int prev_current_time = 0, current_time = 0;
    int completed = 0;
    for (int i = 0; i < NUM_PROC; i++) {
        proc_table[i].remaining_time = proc_table[i].burst_time;
    }

    // Process scheduling loop
    int selected_process, prev_process = -1;
    while (completed != NUM_PROC) {
        selected_process = -1;

        // Find the process with the highest priority that has arrived and not completed yet
        for (int i = 0; i < NUM_PROC; i++) {
            if (proc_table[i].arrival_time <= current_time && \
                    proc_table[i].remaining_time > 0) {
                // Select the process with the highest priority
                if (selected_process == -1 || \
                        proc_table[i].priority < proc_table[selected_process].priority) {
                    selected_process = i;
                } else if (selected_process != -1 && \
                        proc_table[i].priority == proc_table[selected_process].priority) {
                    // Select the process that arrives first if processes have same priority
                    if (proc_table[i].arrival_time < proc_table[selected_process].arrival_time) {
                        selected_process = i;
                    } else if (proc_table[i].arrival_time == proc_table[selected_process].arrival_time && \
                            proc_table[i].pid < proc_table[selected_process].pid) {
                        // Select the process that has lower pid if processes have same priority and arrival time
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
        } else {
            // Execute the selected process for the entire burst time
            current_time += proc_table[selected_process].remaining_time;
            proc_table[selected_process].remaining_time = 0;
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
    proc_sch_time[proc_sch_index * 2] = -1;
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

    // Draw gantt chart
    visualise_gantt(proc_sch_order, proc_sch_time, proc_sch_index);
#pragma endregion PROC_SCH_CALC

    return proc_sch_table;
}
