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
    float total_turnaround_time = 0;
    float total_waiting_time = 0;
    float total_response_time = 0;

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

    bool needs_enqueue = false;
    struct process* running_proc;
    while (completed != NUM_PROC) {
        // Enqueue the processes that has arrived and not completed yet
retry:
        for (int i = 0; i < NUM_PROC; i++) {
            if (proc_started[i]) continue;
            for (int j = prev_current_time; j <= current_time; j++) {
                if (proc_table[i].arrival_time <= j && \
                        proc_table[i].remaining_time > 0) {
                    enqueue(&rr_queue, &proc_table[i]);
                    proc_started[i] = true;
                    queue_len++;
                }
            }
            // Step current_time if all processes haven't arrived
            if (i == NUM_PROC - 1 && isEmpty(&rr_queue)) {
                current_time++;
                goto retry;
            }
        }

        // Enqueue process from previous execution
        if (needs_enqueue)
            enqueue(&rr_queue, running_proc);

        // Run processes in round robin process queue
        running_proc = dequeue(&rr_queue);
        if (proc_table[running_proc->pid].remaining_time <= q_val && \
                proc_table[running_proc->pid].remaining_time > 0) {
            // Execute the selected process for its remaining time
            current_time += running_proc->remaining_time;
            proc_table[running_proc->pid].remaining_time = 0;
            queue_len--;
            completed++;
            needs_enqueue = false;
        } else {
            // Execute the selected process for one quantum unit of time
            proc_table[running_proc->pid].remaining_time = running_proc->remaining_time - q_val;
            current_time += q_val;
            needs_enqueue = true;
        }

        // Save order of process schedule and start and end time
        proc_sch_order[proc_sch_index] = running_proc->pid;
        proc_sch_time[proc_sch_index * 2] = prev_current_time;
        proc_sch_time[proc_sch_index * 2 + 1] = current_time;
        proc_sch_index++;
        prev_current_time = current_time;
    }
    proc_sch_time[proc_sch_index * 2] = -1;
#pragma endregion PROC_SCH_ALGO

#pragma region PROC_SCH_CALC
    float* proc_sch_table = (float *)malloc(3 * sizeof(float));

    // Calculate turnaround time
    for (int i = 0; i < NUM_PROC; i++) {
        proc_table[i].turnaround_time = 0;
        for (int j = proc_sch_index - 1; j >= 0; j--) {
            if (proc_sch_order[j] == i) {
                proc_table[i].turnaround_time += proc_sch_time[j * 2 + 1] - proc_table[i].arrival_time;
                break;
            }
        }
        total_turnaround_time += proc_table[i].turnaround_time;
    }

    // Calculate waiting time
    for (int i = 0; i < NUM_PROC; i++) {
        proc_table[i].waiting_time = proc_table[i].turnaround_time - proc_table[i].burst_time;
        total_waiting_time += proc_table[i].waiting_time;
    }

    // Calculate response time
    for (int i = 0; i < NUM_PROC; i++) {
        for (int j = 0; j < proc_sch_index; j++) {
            if (proc_sch_order[j] == i) {
                total_response_time += proc_sch_time[j * 2] - proc_table[i].arrival_time;
                break;
            }
        }
    }

    proc_sch_table[0] = total_turnaround_time / NUM_PROC;
    proc_sch_table[1] = total_waiting_time / NUM_PROC;
    proc_sch_table[2] = total_response_time / NUM_PROC;

    // Draw gantt chart
    visualise_gantt(proc_sch_order, proc_sch_time, proc_sch_index);
    free(proc_started);
    free(proc_sch_order);
    free(proc_sch_time);
#pragma endregion PROC_SCH_CALC

    return proc_sch_table;
}
