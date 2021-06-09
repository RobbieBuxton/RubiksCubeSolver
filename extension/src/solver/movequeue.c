#include "movequeue.h"

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

static void sift_up(size_t start) {
    if (start <= 0u) {
        // We are at the
        return;
    }
}

bool add_to_move_priority_queue(MovePriorityQueue queue, CubeState state, uint64_t heuristic_value) {
    // TODO
    return false;
}

