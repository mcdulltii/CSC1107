#include <stdio.h>
#include <malloc.h>

#ifndef GANTT_H
#define GANTT_H

void gantt(int* l) {
    for (int c = 0; *l >= 0; c = !c, l++) {
        if (!c) l[1] -= *l;
        while (l[0]-- > 0) putchar(c? 45 : 32);
        c? putchar(10) : 0;
    }
}

void visualise_gantt(int* proc_sch_order, int* proc_sch_time, int array_len) {
    // Draw gantt chart
    gantt(proc_sch_time);

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
    printf("%s", ft_to_string(gantt_table));
    // Destroy table
    ft_destroy_table(gantt_table);
#pragma endregion GANTT_TABLE
}

#endif

