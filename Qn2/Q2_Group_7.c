#include <stdio.h>
#include <stdlib.h>
#include "init.c"
#include "fort.c"

int main(int argc, char *argv[]) {
    int** proc_attr_table = init();
    int* arrival_time = proc_attr_table[0];
    int* burst_time = proc_attr_table[1];
    int* priority = proc_attr_table[2];

    // Create process attribute table
    ft_table_t *attr_table = ft_create_table();

    // Setup header
    ft_set_cell_prop(attr_table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_write_ln(attr_table, "Process", "Arrival Time", "Burst Time", "Priority");

    // Build table values
    char i_str[3], arrival_time_str[2], burst_time_str[3], priority_str[2];
    for (int i=0; i<NUM_PROC; i++) {
        sprintf(i_str, "P%d", i);
        sprintf(arrival_time_str, "%d", arrival_time[i]);
        sprintf(burst_time_str, "%d", burst_time[i]);
        sprintf(priority_str, "%d", priority[i]);
        ft_write_ln(attr_table, i_str, arrival_time_str, burst_time_str, priority_str);
    }

    // Print table
    printf("%s", ft_to_string(attr_table));
    // Destroy table
    ft_destroy_table(attr_table);

    free(arrival_time);
    free(burst_time);
    free(priority);
    free(proc_attr_table);
}
