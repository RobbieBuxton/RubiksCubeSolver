#include "movequeue.h"

#include <string.h>

static inline size_t left_child(size_t curr) {
    return (curr << 1u) + 1u;
}

static inline size_t right_child(size_t curr) {
    return left_child(curr) + 1u;
}

static inline size_t parent(size_t curr) {
    return (curr - 1u) >> 1u;
}

MovePriorityQueue *new_move_priority_queue(size_t initial_size) {
    MovePriorityQueue *queue = (MovePriorityQueue *) malloc(sizeof(MovePriorityQueue));
    if (!queue) {
        return NULL;
    }

    queue->state_queue = (MoveQueueNode *) calloc(initial_size, sizeof(MoveQueueNode));
    if (!queue->state_queue) {
        free(queue);
        return NULL;
    }

    queue->size = initial_size;
    queue->count = 0u;
    queue->error = MQ_OK;

    return queue;
}

bool free_move_priority_queue(MovePriorityQueue* queue) {
    if (!queue) {
        return false;
    }

    free(queue->state_queue);
    free(queue);

    return true;
}

static void sift_up(MovePriorityQueue *queue, size_t start) {
    if (start <= 0u) {
        // We are at the heap root. Stop sifting.
        return;
    }

    // This is a min heap, so swap if current < parent.
    size_t par = parent(start);
    if (queue->state_queue[start].cost < queue->state_queue[par].cost) {
        MoveQueueNode temp = queue->state_queue[par];
        queue->state_queue[par] = queue->state_queue[start];
        queue->state_queue[start] = temp;

        sift_up(queue, par);
    }
}

static void sift_down(MovePriorityQueue *queue, size_t root) {
    if (root >= queue->count) {
        // We've exited the heap.
        return;
    }

    size_t left = left_child(root);
    size_t right = right_child(root);

    size_t to_swap;

    if (right >= queue->count) {
        // Right is out of heap, just use left.
        to_swap = left;
    } else if (left >= queue->count) {
        // Both left and right are our of heap, exit.
        return;
    } else {
        // Both children are in the heap, check as normal.
        // Swap with smaller child...
        to_swap = (queue->state_queue[left].cost <= queue->state_queue[right].cost) ? left : right;
    }

    if (queue->state_queue[root].cost > queue->state_queue[to_swap].cost) {
        // Swap if larger.
        MoveQueueNode temp = queue->state_queue[to_swap];
        queue->state_queue[to_swap] = queue->state_queue[root];
        queue->state_queue[root] = temp;

        sift_up(queue, to_swap);
    }
}

static bool extend_move_priority_queue(MovePriorityQueue *queue) {
    size_t new_size = queue->size + (queue->size >> 1u);
    new_size = (new_size == queue->size) ? new_size + 1u : new_size;

    MoveQueueNode *new_queue = (MoveQueueNode *) realloc(queue->state_queue, new_size * sizeof(MoveQueueNode));
    if (!new_queue) {
        queue->error = MQ_ALLOC_ERROR;
        return false;
    }

    queue->state_queue = new_queue;
    queue->size = new_size;
    queue->error = MQ_OK;

    return true;
}

bool add_to_move_priority_queue(MovePriorityQueue *queue, const CubeState *state, const uint64_t cost) {
    if (queue->count >= queue->size) {
        // Extend the queue first.
        if (!extend_move_priority_queue(queue)) {
            return false;
        }
    }

    size_t where = (queue->count)++;
    memcpy(&queue->state_queue[where].state, state, sizeof(CubeState));
    queue->state_queue[where].cost = cost;
    queue->state_queue[where].hash = hash_cubestate(state);
    queue->error = MQ_OK;

    sift_up(queue, where);

    return true;
}

bool poll_move_priority_queue(MovePriorityQueue *queue, MoveQueueNode *out_node) {
    if (queue->count == 0u) {
        queue->error = MQ_UNDERFLOW;
        return false;
    }

    size_t last = --queue->count;
    memcpy(out_node, queue->state_queue, sizeof(MoveQueueNode));

    queue->state_queue[0u] = queue->state_queue[last];
    sift_down(queue, 0u);

    return true;
}

