#ifndef ATTR_H
#define ATTR_H

struct process {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
    int start_time;
    int end_time;
    int turnaround_time;
    int waiting_time;
    int response_time;
};

#endif

