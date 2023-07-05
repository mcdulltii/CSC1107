#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h>

// Libraries
#include "fort.c"

// Initializations
#include "init.c"
#include "attr.h"
#include "Q2_Group_7.h"

int main(int argc, char *argv[]) {
    // Which process scheduling algorithm to use
    int rr_quantum = 2;
    bool is_preempt = false;
    int scheduling_selection = _get_valid_selection() - 1;
    // Get quantum time if Round Robin scheduling algorithm is selected
    if (scheduling_selection == 3)
        rr_quantum = _get_rr_quantum();
    // Prompt for preemption if Priority scheduling algorithm is selected
    else if (scheduling_selection == 4)
        is_preempt = _get_preempt_selection();

    // Whether to generate process attributes randomly
    int** proc_attr_table;
    if (_get_user_input()) {
        // Generate process attributes randomly
        proc_attr_table = init(scheduling_selection, rr_quantum, is_preempt);
    } else {
        // Retrieve process attributes from input
        proc_attr_table = init_usr();
    }
    int* arrival_time = proc_attr_table[0];
    int* burst_time = proc_attr_table[1];
    int* priority = proc_attr_table[2];
    struct process proc_table[NUM_PROC];

#pragma region PROC_ATTR_TABLE
    // Create process attribute table
    ft_table_t *attr_table = ft_create_table();

    // Setup header
    ft_set_cell_prop(attr_table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_write_ln(attr_table, "Process", "Arrival Time", "Burst Time", "Priority");

    // Build table values
    char i_str[3], arrival_time_str[2], burst_time_str[3], priority_str[2];
    for (int i=0; i<NUM_PROC; i++) {
        sprintf(i_str, "P%d", i);
        proc_table[i].pid = i;
        proc_table[i].start_time = -1;
        sprintf(arrival_time_str, "%d", arrival_time[i]);
        proc_table[i].arrival_time = arrival_time[i];
        sprintf(burst_time_str, "%d", burst_time[i]);
        proc_table[i].burst_time = burst_time[i];
        sprintf(priority_str, "%d", priority[i]);
        proc_table[i].priority = priority[i];
        ft_write_ln(attr_table, i_str, arrival_time_str, burst_time_str, priority_str);
    }

    // Print table
    print(ft_to_string(attr_table), OUTPUT_FILE);
    // Destroy table
    ft_destroy_table(attr_table);
#pragma endregion PROC_ATTR_TABLE

#pragma region PROC_SCH_SELECTION
    float** proc_sch_table = (float**)malloc(NUM_ALGO * sizeof(float*));
    switch(scheduling_selection) {
        case 0:
            // FCFS Scheduling
            proc_sch_table[scheduling_selection] = _run_fcfs_scheduling(proc_table);
            break;
        case 1:
            // SJF Scheduling
            proc_sch_table[scheduling_selection] = _run_sjf_scheduling(proc_table);
            break;
        case 2:
            // SRTF Scheduling
            proc_sch_table[scheduling_selection] = _run_srtf_scheduling(proc_table);
            break;
        case 3:
            // RR Scheduling
            proc_sch_table[scheduling_selection] = _run_rr_scheduling(proc_table, rr_quantum);
            break;
        case 4:
            // Priority Scheduling
            if (is_preempt)
                scheduling_selection++;
            proc_sch_table[scheduling_selection] = _run_priority_scheduling(proc_table, is_preempt);
            break;
        default:
            printf("Invalid selection!\n");
            exit(-1);
    }

    // Run rest of process scheduling algorithms
    for (int i = 0; i < NUM_ALGO; i++) {
        if (i == scheduling_selection) continue;
        _revert_proc_table(proc_table);
        // FCFS, SJF, SRTF scheduling algorithms
        if (i < 3) {
            switch(i) {
                case 0:
                    // FCFS Scheduling
                    proc_sch_table[i] = _run_fcfs_scheduling(proc_table);
                    break;
                case 1:
                    // SJF Scheduling
                    proc_sch_table[i] = _run_sjf_scheduling(proc_table);
                    break;
                case 2:
                    // SRTF Scheduling
                    proc_sch_table[i] = _run_srtf_scheduling(proc_table);
                    break;
            }
        } else if (i == 3) {
            // RR Scheduling with default quantum of 2
            proc_sch_table[i] = _run_rr_scheduling(proc_table, 2);
        } else if (i == 4) {
            // Non-Preemptive Priority Scheduling
            proc_sch_table[i] = _run_priority_scheduling(proc_table, false);
        } else if (i == 5) {
            // Preemptive Priority Scheduling
            proc_sch_table[i] = _run_priority_scheduling(proc_table, true);
        }
    }
#pragma endregion PROC_SCH_SELECTION

#pragma region PROC_SCH_COMPARE
    float shortest_turnaround_time = FLT_MAX;
    float shortest_waiting_time = FLT_MAX;
    float shortest_response_time = FLT_MAX;
    int shortest_proc_sch[3] = {-1, -1, -1};
    // *proc_sch_table = [avg_turnaround_time, avg_waiting_time, avg_response_time]
    for (int i = 0; i < NUM_ALGO; i++) {
        // Get process scheduling algorithm with the shortest turnaround time
        if (proc_sch_table[i][0] < shortest_turnaround_time) {
            shortest_turnaround_time = proc_sch_table[i][0];
            shortest_proc_sch[0] = i;
        }
        // Get process scheduling algorithm with the shortest waiting time
        if (proc_sch_table[i][1] < shortest_waiting_time) {
            shortest_waiting_time = proc_sch_table[i][1];
            shortest_proc_sch[1] = i;
        }
        // Get process scheduling algorithm with the shortest response time
        if (proc_sch_table[i][2] < shortest_response_time) {
            shortest_response_time = proc_sch_table[i][2];
            shortest_proc_sch[2] = i;
        }
    }

    // Print which process scheduling algorithm has the shortest turnaround time, shortest waiting time, and shortest response time
    char* sch_str[NUM_ALGO] = {
        "First-Come, First-Served Scheduling",
        "Shortest-Job-First Scheduling",
        "Shortest Remaining Time First Scheduling",
        "Round Robin Scheduling",
        "Non-Preemptive Priority Scheduling",
        "Preemptive Priority Scheduling"
    };
    char turnaround_time_str[100], waiting_time_str[100], response_time_str[100];
    sprintf(turnaround_time_str, "\n%s has the shortest average turnaround time\n", sch_str[shortest_proc_sch[0]]);
    sprintf(waiting_time_str, "%s has the shortest average waiting time\n", sch_str[shortest_proc_sch[1]]);
    sprintf(response_time_str, "%s has the shortest average response time\n", sch_str[shortest_proc_sch[2]]);
    print(turnaround_time_str, OUTPUT_FILE);
    print(waiting_time_str, OUTPUT_FILE);
    print(response_time_str, OUTPUT_FILE);
#pragma endregion PROC_SCH_COMPARE

    free(arrival_time);
    free(burst_time);
    free(priority);
    free(proc_attr_table);
    for (int i = 0; i < NUM_ALGO; i++) free(proc_sch_table[i]);
    free(proc_sch_table);
}

