#include "movequeue.h"

MovePriorityQueue *new_move_priority_queue(size_t initial_size) {
    MovePriorityQueue *queue = (MovePriorityQueue *) malloc(sizeof(MovePriorityQueue));
    if (!queue) {
        return NULL;
    }

    queue->state_queue = (CubeState *) calloc(initial_size, sizeof(MoveQueueNode));
    if (!queue->state_queue) {
        free(queue);
        return NULL;
    }

    queue->size = initial_size;
    queue->count = 0u;

    return queue;
}

