#ifndef ATTR_H
#define ATTR_H

#define NUM_PROC 6

int** gen_attr();
int* _gen_rand_table(int, int, int, int);
int* _gen_arrival_time();
int* _gen_burst_time();
int* _gen_priority();

#endif

