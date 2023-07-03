#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Scheduling algorithms
#include "fcfs.c"
#include "priority.c"
#include "srtf.c"
#include "sjf.c"
#include "rr.c"

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

float* _run_fcfs_scheduling(struct process* proc_table) {
    char* fcfs_str = "\nFirst-Come, First-Served Scheduling Algorithm:\n";
    print(fcfs_str, OUTPUT_FILE);
    // *proc_sch_table = [avg_turnaround_time, avg_waiting_time, avg_response_time]
    float* proc_sch_table = fcfs_scheduling(proc_table);
    _print_proc_table(proc_sch_table);
    return proc_sch_table;
}

float* _run_sjf_scheduling(struct process* proc_table) {
    char* sjf_str = "\nShortest-Job-First Scheduling Algorithm:\n";
    print(sjf_str, OUTPUT_FILE);
    // *proc_sch_table = [avg_turnaround_time, avg_waiting_time, avg_response_time]
    float* proc_sch_table = sjf_scheduling(proc_table);
    _print_proc_table(proc_sch_table);
    return proc_sch_table;
}

float* _run_srtf_scheduling(struct process* proc_table) {
    char* srtf_str = "\nShortest Remaining Time First Scheduling Algorithm:\n";
    print(srtf_str, OUTPUT_FILE);
    // *proc_sch_table = [avg_turnaround_time, avg_waiting_time, avg_response_time]
    float* proc_sch_table = srtf_scheduling(proc_table);
    _print_proc_table(proc_sch_table);
    return proc_sch_table;
}

float* _run_rr_scheduling(struct process* proc_table, int rr_quantum) {
    char* rr_str = "\nRound Robin Scheduling Algorithm:\n";
    print(rr_str, OUTPUT_FILE);
    // *proc_sch_table = [avg_turnaround_time, avg_waiting_time, avg_response_time]
    float* proc_sch_table = rr_scheduling(proc_table, rr_quantum);
    _print_proc_table(proc_sch_table);
    return proc_sch_table;
}

float* _run_priority_scheduling(struct process* proc_table, bool is_preempt) {
    char* priority_str = is_preempt ? "\nPreemptive Priority Scheduling Algorithm:\n"
                                    : "\nNon-Preemptive Priority Scheduling Algorithm:\n";
    print(priority_str, OUTPUT_FILE);
    // *proc_sch_table = [avg_turnaround_time, avg_waiting_time, avg_response_time]
    float* proc_sch_table = priority_scheduling(proc_table, is_preempt);
    _print_proc_table(proc_sch_table);
    return proc_sch_table;
}

void _revert_proc_table(struct process* proc_table) {
    // Sort process table by PID
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

