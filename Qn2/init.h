#ifndef INIT_H
#define INIT_H

#define NUM_ALGO 6
#define NUM_PROC 6
#define OUTPUT_FILE "Q2_Group_7.txt"

struct time {
    int value;
    int index;
};

void print(const char*, char*);
int** init(int, int, bool);
int** init_usr();
int cmp(const void*, const void*);
int* _gen_rand_table(int, int, int, int);
int* _gen_arrival_time();
int* _gen_burst_time();
int* _gen_priority();
int* _get_attr_values(const char*);
int* _gen_fcfs_arrival_time(int*);

#endif

