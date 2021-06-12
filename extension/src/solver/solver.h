#ifndef __SOLVER_H__
#define __SOLVER_H__

#include "cubestate.h"
#include "movequeue.h"

#define MATCHES_CENTRE(f, r, c, cube) (cube->data[f][r][c] == cube->data[f][1][1])
#define MISPLACED_CORNER(f, r, c, cube) (cube->data[f][r][c] != cube->data[f][r][1] && cube->data[f][r][c] != cube->data[f][1][c])

/**
 * "Colours" for the Hash nodes.
 * Used for balancing a hash tree.
 */
typedef enum {
    RED_NODE,  /**< Red colouring for a node. */
    BLACK_NODE /**< Black colouring for a node. */
} NodeColour;

/**
 * A Node for a hash bst.
 */
typedef struct MapNode_t {
    uint64_t hash;                 /**< Hash identifier of node. */

    NodeColour colour;             /**< hash colour for use in balancing. */
    struct MapNode_t *parent;      /**< Parent hash for this hash. */
    struct MapNode_t *left_child;  /**< Left child for this hash. */
    struct MapNode_t *right_child; /**< Right child for this hash. */
} MapNode;


/**
 * An red-black tree of uint64_t
 */
typedef struct {
    size_t count;  /**< Number of items currently stored in the tree. */
    MapNode *root; /**< Pointer to the root of the map's hash tree. */
} HashTree;

/** 
 * Finds a solution set of moves for a cube starting in position represented by start.
 * 
 * @param[in]   start       The starting position, history should be empty.
 * @param[out]  move_count  The number of moves in the solution.
 * @param[out]  solution    An array of moves which transform start to a solved cube
 * @return                  True if a solution was found.
 * 
 */
bool solve(CubeState *start, int *move_count, Movement* solution);

/**
 * Calculates estimated distance from state to a solved state.
 * 
 * @param state The state to calculate a heuristic for
 * @return      The heuristic value
 * 
 */
double heuristic(CubeState *state);

/**
 * Calculates estimated cost of a solution through state.
 * 
 * @param state The state to calculate a heuristic for
 * @return      The estimated cost.
 * 
 */
double estimate_cost(CubeState *state);

/**
 * Adds all states reachable in a single move from current to queue which have not yet been visited
 * 
 * @param[in]  current          The state to move from.
 * @param[out] queue            The queue to add the new states too.
 * @param[in]  visitedHashes    A searchable array of hashes that should not be re-added.
 * @return                      True if the state was successfully expanded.
 *
 */
bool expand_all_moves(CubeState *current, MovePriorityQueue *queue, HashTree *visitedHashes);

/**
 * Checks whether the hash for current state is in visitedHashes
 * and adds it if not there
 * 
 * @param   current         The state to check.
 * @param   visitedHashes   A searchable array of hashes.
 * @return                  True if it has been added (it was not previously visited).
 * 
 */
bool visit(CubeState *current, HashTree *visitedHashes);


/**
 * Allocate a new hash map. This map must be freed later using free_hash_tree.
 *
 * @return              If successful, the pointer to the new hash tree.
 */
HashTree *new_hash_tree(void);

/**
 * Free a hash tree created by new_hash_tree.
 *
 * @param  map The map to free
 * @return     Success in freeing the tree.
 */
bool free_hash_tree(HashTree *map);

/**
 * Add a hash to the tree.
 * Failing to add a hash does not free the tree.
 *
 * @param[out] map    Hash tree to add hash to
 * @param[in]  hash   Hash to add.
 * @return            True if hash was sucessfully added. False otherwise.
 */
bool add_to_hash_tree(HashTree* map, uint64_t hash);

/**
 * Query a map for a Hash
 * Uses binary search, for a tree traversal, O(log_2 n).
 *
 * @param  map         The map to query
 * @param  hash        The hash to find
 * @return             True if it is present
 */
bool query_hash_tree(const HashTree *map, const uint64_t hash);


#endif