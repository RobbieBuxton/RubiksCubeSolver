#ifndef __HASHTREE_H__
#define __HASHTREE_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

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

#endif  // __HASHTREE_H__

