#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Libraries
#include "fort.c"

// Initializations
#include "init.c"
#include "attr.h"

// Scheduling algorithms
#include "priority.c"
#include "srtf.c"

bool get_user_input();
int get_valid_selection();

int main(int argc, char *argv[]) {
    // Whether to generate process attributes randomly
    if (get_user_input()) {
        // Generate process attributes randomly
        int** proc_attr_table = init();
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
            proc_table[i].pid = i+1;
            sprintf(arrival_time_str, "%d", arrival_time[i]);
            proc_table[i].arrival_time = arrival_time[i];
            sprintf(burst_time_str, "%d", burst_time[i]);
            proc_table[i].burst_time = burst_time[i];
            sprintf(priority_str, "%d", priority[i]);
            proc_table[i].priority = priority[i];
            ft_write_ln(attr_table, i_str, arrival_time_str, burst_time_str, priority_str);
        }

        // Print table
        printf("%s", ft_to_string(attr_table));
        // Destroy table
        ft_destroy_table(attr_table);
#pragma endregion PROC_ATTR_TABLE

#pragma region PROC_SCH_SELECTION
        // *proc_sch_table = [avg_turnaround_time, avg_waiting_time, avg_response_time]
        float* proc_sch_table;
        // Which process scheduling algorithm to use
        switch(get_valid_selection()) {
            case 1:
                // FCFS Scheduling
                break;
            case 2:
                // SJF Scheduling
                break;
            case 3:
                // SRTF Scheduling
                proc_sch_table = srtf_scheduling(proc_table);
                break;
            case 4:
                // RR Scheduling
                break;
            case 5:
                // Priority Scheduling
                proc_sch_table = priority_scheduling(proc_table);
                break;
            default:
                printf("Invalid selection!");
                return -1;
        }
#pragma endregion PROC_SCH_SELECTION

#pragma region PROC_SCH_TABLE
#pragma endregion PROC_SCH_TABLE

        free(arrival_time);
        free(burst_time);
        free(priority);
        free(proc_attr_table);
        free(proc_sch_table);
    }
}

bool get_user_input() {
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

int get_valid_selection() {
    char input[5];
    int value;
retry_get_valid_selection:
    printf("Choose a process selection algorithm [1-5]: ");
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

