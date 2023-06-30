#ifndef INIT_H
#define INIT_H

#define NUM_PROC 6
#define OUTPUT_FILE "Q2_Group_7.txt"

void print(const char*, char*);
int** init();
int** init_usr();
int* _gen_rand_table(int, int, int, int);
int* _gen_arrival_time();
int* _gen_burst_time();
int* _gen_priority();
int* _get_attr_values(const char*);

#endif

