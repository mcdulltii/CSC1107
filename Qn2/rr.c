#include "malloc.h"
#include "init.h"
#include "attr.h"
#include "gantt.h"
#define MAX_SIZE 12

typedef struct {
    struct process* q_proc_table[MAX_SIZE];
    int front;
    int rear;
} SimpleQueue;

void start(SimpleQueue* queue) {
    queue->front = -1;
    queue->rear = -1;
}

bool isEmpty(SimpleQueue* queue) {
    return (queue->front == -1 && queue->rear == -1);
}

bool isFull(SimpleQueue* queue) {
    return ((queue->rear + 1) % MAX_SIZE == queue->front);
}

void enqueue(SimpleQueue* queue, struct process* q_proc_table) {
    if (isFull(queue)) {
        printf("Queue is full. Cannot enqueue element.\n");
    }

    if (isEmpty(queue)) {
        queue->front = 0;
        queue->rear = 0;
    } else {
        queue->rear = (queue->rear + 1) % MAX_SIZE;
    }

    queue->q_proc_table[queue->rear] = q_proc_table;
}

struct process* dequeue(SimpleQueue* queue) {

    struct process* empty = NULL;

    if (isEmpty(queue)) {
        printf("Queue is empty. Cannot dequeue element.\n");
        return empty;
    }

    struct process* q_proc_table = queue->q_proc_table[queue->front];

    if (queue->front == queue->rear) {
        queue->front = -1;
        queue->rear = -1;
    } else {
        queue->front = (queue->front + 1) % MAX_SIZE;
    }

    return q_proc_table;
}


float* rr_scheduling(struct process* proc_table, int q_val) {
    float* proc_sch_table = (float *)malloc(3 * sizeof(float));
    bool* proc_started = (bool *)calloc(NUM_PROC, sizeof(bool));
    int proc_sch_index = 0;
    int* proc_sch_order = (int *)malloc(NUM_PROC * 5 * sizeof(int));
    int* proc_sch_time = (int *)malloc(NUM_PROC * 5 * sizeof(int));

#pragma region PROC_SCH_ALGO
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    int total_response_time = 0;
    float throughput;

    int current_time, prev_current_time = 0;
    int completed = 0;

    int queue_len = 0;
    SimpleQueue rr_queue;
    start(&rr_queue);

    // keep track of remaining burst time for each process
    for (int i = 0; i < NUM_PROC; i++) {
        proc_table[i].remaining_time = proc_table[i].burst_time;
    }
    for (int i = 0; i < NUM_PROC; i++) {
        proc_started[i] = false;
    }

    int selected_process;
    // algorithm starts here
    while (completed != NUM_PROC) {
        selected_process = -1;

        // Find the process with the earliest arrival time that has arrived and not completed yet
        for (int i = 0; i < NUM_PROC; i++) {
            if (proc_started[i]) continue;
            if (proc_table[i].arrival_time <= current_time && \
                    proc_table[i].remaining_time > 0) {
                    selected_process = i;
                    enqueue(&rr_queue, &proc_table[selected_process]);
                    proc_started[selected_process] = true;
                    queue_len++;
            }
        }

        if (current_time > 100) break;
        // No processes selected, step forward time unit
        if (selected_process == -1) {
            current_time++;
            continue;
        }

        while (!isEmpty(&rr_queue)) {
            struct process* running_proc = dequeue(&rr_queue);
            if (proc_table[running_proc->pid].remaining_time <= q_val && \
                    proc_table[running_proc->pid].remaining_time > 0) {
                // Execute the selected process for the entire burst time
                current_time += running_proc->remaining_time;
                proc_table[running_proc->pid].remaining_time = 0;
                queue_len--;
                completed++;

                // Save order of process schedule and start and end time
                proc_sch_order[proc_sch_index] = running_proc->pid;
                proc_sch_time[proc_sch_index * 2] = prev_current_time;
                proc_sch_time[proc_sch_index * 2 + 1] = current_time;
                proc_sch_index++;
                prev_current_time = current_time;
            } else {
                // Execute the selected process for one quantum unit of time
                proc_table[running_proc->pid].remaining_time = running_proc->remaining_time - q_val;
                current_time += q_val;
                enqueue(&rr_queue, running_proc);

                // Save order of process schedule and start and end time
                proc_sch_order[proc_sch_index] = running_proc->pid;
                proc_sch_time[proc_sch_index * 2] = prev_current_time;
                proc_sch_time[proc_sch_index * 2 + 1] = current_time;
                proc_sch_index++;
                prev_current_time = current_time;
            }

            // Check for processes that will be arriving
            for (int i = 0; i < NUM_PROC; i++) {
                if (proc_started[i]) continue;
                if (proc_table[i].arrival_time <= current_time) goto get_next_process;
            }

            if (completed == NUM_PROC) goto end_algo;
        }

get_next_process:
        // Update start time and end time for the process if it just started or completed
        // if (proc_table[selected_process].start_time == -1) {
        //     proc_table[selected_process].start_time = current_time - 1;
        // }
        // if (proc_table[selected_process].remaining_time == 0) {
        //     proc_table[selected_process].end_time = current_time;
        //     completed++;
        // }
    }
#pragma endregion PROC_SCH_ALGO

end_algo:
    
    for (int i = 0; i < NUM_PROC; i++) {
        proc_table[i].turnaround_time = proc_table[i].end_time - proc_table[i].arrival_time;
        proc_table[i].waiting_time = proc_table[i].turnaround_time - proc_table[i].burst_time;
        proc_table[i].response_time = proc_table[i].start_time - proc_table[i].arrival_time;
        total_turnaround_time += proc_table[i].turnaround_time;
        total_waiting_time += proc_table[i].waiting_time;
        total_response_time += proc_table[i].response_time;
    }

    proc_sch_table[0] = total_turnaround_time / NUM_PROC;
    proc_sch_table[1] = total_waiting_time / NUM_PROC;
    proc_sch_table[2] = total_response_time / NUM_PROC;

    // Draw gantt chart
    visualise_gantt(proc_sch_order, proc_sch_time, proc_sch_index);
    free(proc_sch_order);
    free(proc_sch_time);

    return proc_sch_table;
}
