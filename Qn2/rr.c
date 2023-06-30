#include "malloc.h"
#include "init.h"
#include "attr.h"
#include "gantt.h"
#define MAX_SIZE 50

// Process queue
typedef struct {
    struct process* q_proc_table[MAX_SIZE];
    int front;
    int rear;
} SimpleQueue;

// Initialize process queue
void start(SimpleQueue* queue) {
    queue->front = -1;
    queue->rear = -1;
}

// Check if process queue is empty
bool isEmpty(SimpleQueue* queue) {
    return (queue->front == -1 && queue->rear == -1);
}

// Check if process queue is full
bool isFull(SimpleQueue* queue) {
    return ((queue->rear + 1) % MAX_SIZE == queue->front);
}

// Enqueue process into process queue
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

// Dequeue process from process queue
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
    bool* proc_started = (bool *)calloc(NUM_PROC, sizeof(bool));
    int proc_sch_index = 0;
    int* proc_sch_order = (int *)malloc(NUM_PROC * 50 * sizeof(int));
    int* proc_sch_time = (int *)malloc(NUM_PROC * 50 * sizeof(int));

#pragma region PROC_SCH_ALGO
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    int total_response_time = 0;

    int current_time = 0, prev_current_time = 0;
    int completed = 0;

    int queue_len = 0;
    // Initialize round robin process queue
    SimpleQueue rr_queue;
    start(&rr_queue);

    // Keep track of remaining burst time for each process
    for (int i = 0; i < NUM_PROC; i++) {
        proc_table[i].remaining_time = proc_table[i].burst_time;
    }
    // Set all processes to waiting (i.e. Not started)
    for (int i = 0; i < NUM_PROC; i++) {
        proc_started[i] = false;
    }

    while (completed != NUM_PROC) {
        // Find the process with the earliest arrival time that has arrived and not completed yet
        for (int i = 0; i < NUM_PROC; i++) {
            if (proc_started[i]) continue;
            if (proc_table[i].arrival_time <= current_time && \
                    proc_table[i].remaining_time > 0) {
                    enqueue(&rr_queue, &proc_table[i]);
                    proc_started[i] = true;
                    queue_len++;
            }
            // Step current_time if all processes haven't arrived
            if (i == NUM_PROC - 1) {
                current_time++;
            }
        }

        // Run processes in round robin process queue
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
                proc_table[running_proc->pid].start_time = prev_current_time;
                proc_table[running_proc->pid].end_time = current_time;
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
                proc_table[running_proc->pid].start_time = prev_current_time;
                proc_table[running_proc->pid].end_time = current_time;
                prev_current_time = current_time;
            }

            // Check for processes that will be arriving
            for (int i = 0; i < NUM_PROC; i++) {
                if (proc_started[i]) continue;
                // Enqueue next process that has arrived
                if (proc_table[i].arrival_time <= current_time)
                    goto get_next_process;
            }
        }
get_next_process:
    }
#pragma endregion PROC_SCH_ALGO

#pragma region PROC_SCH_CALC
    float* proc_sch_table = (float *)malloc(3 * sizeof(float));

    // Calculate turnaround time, waiting time, and response time
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
#pragma endregion PROC_SCH_CALC

    return proc_sch_table;
}
