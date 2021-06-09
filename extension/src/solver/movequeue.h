#ifndef __MOVEQUEUE_H__
#define __MOVEQUEUE_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "cubestate.h"

typedef struct {
    CubeState state;
    uint64_t hash;
    uint64_t heuristic_value;
} MoveQueueNode;

// This is a min-heap priority queue.

typedef struct {
    size_t size;
    size_t count;
    MoveQueueNode *state_queue;
} MovePriorityQueue;

MovePriorityQueue *new_move_priority_queue(size_t initial_size);

bool free_move_priority_queue(MovePriorityQueue* queue);

bool add_to_move_priority_queue(MovePriorityQueue queue, CubeState state, uint64_t heuristic_value);

#endif  // __MOVEQUEUE_H__

