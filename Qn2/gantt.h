#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "init.h"

#ifndef GANTT_H
#define GANTT_H

void visualise_gantt(int* proc_sch_order, int* proc_sch_time, int array_len) {
#pragma region TIME_TABLE
    // Create time table
    ft_table_t *time_table = ft_create_table();

    // Setup header
    ft_set_cell_prop(time_table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_write_ln(time_table, "Gantt Chart");

    // Iterate process scheduled time and print inline with process order
    int prev_time = 0;
    char gantt_str[500] = {};
    char space_char[] = " ", dash_char[] = "-", newline_char[] = "\n";
    for (int i=0; i<NUM_PROC; i++) {
        prev_time = 0;
        for (int j=0; j<array_len; j++) {
            if (proc_sch_order[j] == i) {
                int* gantt_time = &proc_sch_time[2 * j];
                // Add spaces as gaps
                for (int k=prev_time; k<gantt_time[0]; k++) strncat(gantt_str, space_char, 1);
                // Add dashes as scheduled process
                for (int k=gantt_time[0]; k<gantt_time[1]; k++) strncat(gantt_str, dash_char, 1);
                // Save last index
                prev_time = gantt_time[1];
            }
        }
        strncat(gantt_str, newline_char, 1);
    }
    ft_write_ln(time_table, gantt_str);

#pragma region GANTT_TABLE
    // Create gantt table
    ft_table_t *gantt_table = ft_create_table();

    // Build table values
    char proc_str[2];
    for (int i=0; i<array_len; i++) {
        sprintf(proc_str, "%d", proc_sch_order[i]);
        ft_set_cur_cell(gantt_table, 0, i);
        ft_write(gantt_table, proc_str);
    }

    // Print table
    ft_write_ln(time_table, ft_to_string(gantt_table));
    ft_set_cell_prop(time_table, 2, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
#pragma endregion GANTT_TABLE

    // Print table
    print(ft_to_string(time_table), OUTPUT_FILE);
    // Destroy table
    ft_destroy_table(gantt_table);
    // Destroy table
    ft_destroy_table(time_table);
#pragma endregion TIME_TABLE
}

#endif

