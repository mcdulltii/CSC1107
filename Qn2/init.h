#ifndef INIT_H
#define INIT_H

#define NUM_ALGO 6
#define NUM_PROC 6
#define OUTPUT_FILE "Q2_Group_7.txt"

#define ARRIVAL_START 0
#define ARRIVAL_END 8
#define ARRIVAL_LIMIT 3
#define BURST_START 3
#define BURST_END 10
#define BURST_LIMIT 3
#define PRIO_START 1
#define PRIO_END 4
#define PRIO_LIMIT 2

struct time {
    int value;
    int value2;
    int index;
};

void print(const char*, char*);
int** init(int, int, bool);
int** init_usr();
int cmp(const void*, const void*);
int cmp2(const void*, const void*);
int* _gen_rand_table(int, int, int, int);
int* _gen_arrival_time(int);
int* _gen_burst_time(int);
int* _gen_priority(int);
int* _get_attr_values(const char*);
int* _gen_fcfs_arrival_time(int*);
int* _gen_sjf_burst_time(int*);
int* _gen_rr_burst_time(int);
int* _gen_preempt_attr(int, int, int*);
int* _gen_nonpreempt_priority(int*);

#endif

