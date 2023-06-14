#include <malloc.h>
#include "init.h"
#include "attr.h"

float* srtf_scheduling(struct process* proc_table) {
    float* proc_sch_table = (float *)malloc(NUM_PROC * sizeof(float));

#pragma region PROC_SCH_ALGO
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    int total_response_time = 0;
    float throughput;

    int current_time = 0;
    int completed = 0;
#pragma endregion PROC_SCH_ALGO

    proc_sch_table[0] = total_turnaround_time / NUM_PROC;
    proc_sch_table[1] = total_waiting_time / NUM_PROC;
    proc_sch_table[2] = total_response_time / NUM_PROC;
    return proc_sch_table;
}
