#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Libraries
#include "fort.c"

// Initializations
#include "init.c"
#include "attr.h"

// Scheduling algorithms
#include "fcfs.c"
#include "priority.c"
#include "srtf.c"
#include "sjf.c"
#include "rr.c"

bool _get_user_input();
int _get_valid_selection();
bool _get_preempt_selection();
int _get_rr_quantum();

void _run_fcfs_scheduling(struct process*);
void _run_sjf_scheduling(struct process*);
void _run_srtf_scheduling(struct process*);
void _run_rr_scheduling(struct process*, int);
void _run_priority_scheduling(struct process*, bool);
void _print_proc_table(float*);
void _revert_proc_table(struct process* proc_table);

int main(int argc, char *argv[]) {
    // Whether to generate process attributes randomly
    int** proc_attr_table;
    if (_get_user_input()) {
        // Generate process attributes randomly
        proc_attr_table = init();
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
    // Which process scheduling algorithm to use
    int scheduling_selection = _get_valid_selection() - 1;
    switch(scheduling_selection) {
        case 0:
            // FCFS Scheduling
            _run_fcfs_scheduling(proc_table);
            break;
        case 1:
            // SJF Scheduling
            _run_sjf_scheduling(proc_table);
            break;
        case 2:
            // SRTF Scheduling
            _run_srtf_scheduling(proc_table);
            break;
        case 3:
            // RR Scheduling
            int rr_quantum = _get_rr_quantum();
            _run_rr_scheduling(proc_table, rr_quantum);
            break;
        case 4:
            // Priority Scheduling
            bool is_preempt = _get_preempt_selection();
            if (is_preempt)
                scheduling_selection++;
            _run_priority_scheduling(proc_table, is_preempt);
            break;
        default:
            printf("Invalid selection!");
            return -1;
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
                    _run_fcfs_scheduling(proc_table);
                    break;
                case 1:
                    // SJF Scheduling
                    _run_sjf_scheduling(proc_table);
                    break;
                case 2:
                    // SRTF Scheduling
                    _run_srtf_scheduling(proc_table);
                    break;
            }
        } else if (i == 3) {
            // RR Scheduling with default quantum of 2
            _run_rr_scheduling(proc_table, 2);
        } else if (i == 4) {
            // Non-Preemptive Priority Scheduling
            _run_priority_scheduling(proc_table, false);
        } else if (i == 5) {
            // Preemptive Priority Scheduling
            _run_priority_scheduling(proc_table, true);
        }
    }
#pragma endregion PROC_SCH_SELECTION

    free(arrival_time);
    free(burst_time);
    free(priority);
    free(proc_attr_table);
}

bool _get_user_input() {
    char input[5];
    char value;
retry_get_user_input:
    printf("Would you like to generate process attributes randomly? [y/n]: ");
    // Get user input
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%c", &value);
    // Check user input is y/n
    if (value != 'y' && value != 'n') {
        printf("Invalid input. Please enter y/n.\n");
        goto retry_get_user_input;
    }
    return value == 'y'; // false is implied
}

int _get_valid_selection() {
    char input[5];
    int value;
retry_get_valid_selection:
    printf("1. First Come First Serve Scheduling\n"
    "2. Shortest Job First Scheduling\n"
    "3. Shortest Remaining Time First Scheduling\n"
    "4. Round Robin Scheduling\n"
    "5. Priority Scheduling\n"
    "Choose a process selection algorithm [1-5]: ");
    // Get user input
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%d", &value);
    // Check user input is 1-5
    if (!(value >= 1 && value <= 5)) {
        printf("Invalid input. Please enter a number from 1-5.\n");
        goto retry_get_valid_selection;
    }
    return value;
}

bool _get_preempt_selection() {
    char input[5];
    char value;
retry_get_preempt_selection:
    printf("Should the priority scheduling algorithm be preemptive? [y/n]: ");
    // Get user input
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%c", &value);
    // Check user input is y/n
    if (value != 'y' && value != 'n') {
        printf("Invalid input. Please enter y/n.\n");
        goto retry_get_preempt_selection;
    }
    return value == 'y'; // false is implied
}

int _get_rr_quantum() {
    char input[5];
    int value;
retry_get_rr_quantum:
    printf("Enter a quantum value: ");
    // Get user input
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%d", &value);
    // Check quantum time is within limit
    if (!(value > 0 && value < 10)) {
        printf("Invalid input. Please enter a number between 0 and 10.\n");
        goto retry_get_rr_quantum;
    }
    return value;
}

void _run_fcfs_scheduling(struct process* proc_table) {
    char* fcfs_str = "\nFirst-Come, First-Served Scheduling Algorithm:\n";
    print(fcfs_str, OUTPUT_FILE);
    // *proc_sch_table = [avg_turnaround_time, avg_waiting_time, avg_response_time]
    float* proc_sch_table = fcfs_scheduling(proc_table);
    _print_proc_table(proc_sch_table);
    free(proc_sch_table);
}

void _run_sjf_scheduling(struct process* proc_table) {
    char* sjf_str = "\nShortest-Job-First Scheduling Algorithm:\n";
    print(sjf_str, OUTPUT_FILE);
    // *proc_sch_table = [avg_turnaround_time, avg_waiting_time, avg_response_time]
    float* proc_sch_table = sjf_scheduling(proc_table);
    _print_proc_table(proc_sch_table);
    free(proc_sch_table);
}

void _run_srtf_scheduling(struct process* proc_table) {
    char* srtf_str = "\nShortest Remaining Time First Scheduling Algorithm:\n";
    print(srtf_str, OUTPUT_FILE);
    // *proc_sch_table = [avg_turnaround_time, avg_waiting_time, avg_response_time]
    float* proc_sch_table = srtf_scheduling(proc_table);
    _print_proc_table(proc_sch_table);
    free(proc_sch_table);
}

void _run_rr_scheduling(struct process* proc_table, int rr_quantum) {
    char* rr_str = "\nRound Robin Scheduling Algorithm:\n";
    print(rr_str, OUTPUT_FILE);
    // *proc_sch_table = [avg_turnaround_time, avg_waiting_time, avg_response_time]
    float* proc_sch_table = rr_scheduling(proc_table, rr_quantum);
    _print_proc_table(proc_sch_table);
    free(proc_sch_table);
}

void _run_priority_scheduling(struct process* proc_table, bool is_preempt) {
    char* priority_str = is_preempt ? "\nPreemptive Priority Scheduling Algorithm:\n"
                                    : "\nNon-Preemptive Priority Scheduling Algorithm:\n";
    print(priority_str, OUTPUT_FILE);
    // *proc_sch_table = [avg_turnaround_time, avg_waiting_time, avg_response_time]
    float* proc_sch_table = priority_scheduling(proc_table, is_preempt);
    _print_proc_table(proc_sch_table);
    free(proc_sch_table);
}

void _print_proc_table(float* proc_sch_table) {
    // Create process scheduling table
    ft_table_t *sch_table = ft_create_table();

    // Setup header
    ft_set_cell_prop(sch_table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_write_ln(sch_table, "Average Turnaround Time", "Average Waiting Time", "Average Response Time");

    // Build table values
    char turnaround_time_str[20], waiting_time_str[20], response_time_str[20];
    sprintf(turnaround_time_str, "%f", proc_sch_table[0]);
    sprintf(waiting_time_str, "%f", proc_sch_table[1]);
    sprintf(response_time_str, "%f", proc_sch_table[2]);
    ft_write_ln(sch_table, turnaround_time_str, waiting_time_str, response_time_str);

    // Print table
    print(ft_to_string(sch_table), OUTPUT_FILE);
    // Destroy table
    ft_destroy_table(sch_table);
}

void _revert_proc_table(struct process* proc_table) {
    for (int i = 0; i < NUM_PROC; i++) {
        for (int j = i + 1; j < NUM_PROC; j++) {
            if (proc_table[j].pid == i) {
                struct process temp = proc_table[i];
                proc_table[i] = proc_table[j];
                proc_table[j] = temp;
            }
        }
    }
}

