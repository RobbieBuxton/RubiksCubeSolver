#ifndef __MOVEQUEUE_H__
#define __MOVEQUEUE_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "cubestate.h"

/**
 * Error state of a movement priority queue.
 */
typedef enum {
    MQ_OK,         /**< All is fine. */
    MQ_UNDERFLOW,  /**< Poll has underflowed. */
    MQ_ALLOC_ERROR /**< Memory allocation has failed. */
} MoveQueueError;

/**
 * Node used in a priority queue.
 */
typedef struct {
    CubeState state; /**< The state of the cube in this node. */
    uint64_t hash;   /**< The hash of the cube state in this node. */
    double cost;     /**< The cost value used for the priority sorting. */
} MoveQueueNode;

// This is a min-heap priority queue.

/**
 * A movememnt priority queue for the solving algorithm.
 */
typedef struct {
    size_t size;                /**< Size of the queue array. */
    size_t count;               /**< Number of nodes in this queue. */
    MoveQueueNode *state_queue; /**< The internal node array. */
    MoveQueueError error;       /**< Error state of the queue. Check this after most operations. */
} MovePriorityQueue;

/**
 * Create a new priority queue with a given start size.
 * The queue will expand as items are added to it.
 *
 * @param  initial_size Initial proposed size of the queue.
 * @return              A pointer to the new queue if successful. NULL otherwise.
 */
MovePriorityQueue *new_move_priority_queue(size_t initial_size);

/**
 * Free a priority queue.
 *
 * @param  queue The queue to free.
 * @return       If queue is NULL, return false. Returns true otherwise.
 */
bool free_move_priority_queue(MovePriorityQueue* queue);

/**
 * Add a state with its heuristic value to the queue.
 *
 * @param[out] queue           Queue to add to.
 * @param[in]  state           Cube state to add.
 * @param[in]  heuristic_value Cube state's movement cost.
 * @return                     If the addition is successful, returns true. Otherwise, returns false.
 */
bool add_to_move_priority_queue(MovePriorityQueue *queue, const CubeState *state, const double cost);

/**
 * Get the item with the lowest heuristic value.
 * This item will be removed from the queue.
 *
 * @param[in]  queue    Queue to poll.
 * @param[out] out_node Output node to copy the information to.
 * @return              True if queue was successfully polled. Returns false otherwise.
 */
bool poll_move_priority_queue(MovePriorityQueue *queue, MoveQueueNode *out_node);

/**
 * Get a pointer to the node in the queue with the given hash.
 * Has the worst case O(n), average case n / 2.
 *
 * @param  queue Queue to query.
 * @param  hash  Hash to look for.
 * @return       A pointer to a node if the hash is found. NULL otherwise.
 */
MoveQueueNode *find_node_in_move_priority_queue(MovePriorityQueue *queue, uint64_t hash);

#endif  // __MOVEQUEUE_H__

