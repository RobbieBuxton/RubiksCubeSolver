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
typedef struct TreeNode_t {
    uint64_t hash;                  /**< Hash identifier of node. */
    void *any_ptr;                  /**< Generic use pointer. Can be allocated to, or to store an existing pointer. */

    NodeColour colour;              /**< hash colour for use in balancing. */
    struct TreeNode_t *parent;      /**< Parent hash for this hash. */
    struct TreeNode_t *left_child;  /**< Left child for this hash. */
    struct TreeNode_t *right_child; /**< Right child for this hash. */
} TreeNode;

/**
 * An red-black tree of uint64_t
 */
typedef struct {
    size_t count;  /**< Number of items currently stored in the tree. */
    TreeNode *root; /**< Pointer to the root of the tree's hash tree. */
} HashTree;

/**
 * Allocate a new hash tree. This tree must be freed later using free_hash_tree.
 *
 * @return If successful, the pointer to the new hash tree.
 */
HashTree *new_hash_tree(void);

/**
 * Free a hash tree created by new_hash_tree.
 *
 * @param  tree The tree to free
 * @return      Success in freeing the tree.
 */
bool free_hash_tree(HashTree *tree);

/**
 * Add a hash to the tree.
 * Failing to add a hash does not free the tree.
 *
 * @param[out] tree Hash tree to add hash to
 * @param[in]  hash Hash to add.
 * @return          True if hash was sucessfully added. False otherwise.
 */
bool add_to_hash_tree(HashTree *tree, uint64_t hash);

/**
 * Modify a hash's pointer association.
 *
 * @param[out] tree Hash tree to modify pointer
 * @param[in]  hash Hash of node to modify
 * @param[in]  ptr  Pointer to use
 * @return          True if pointer successfully modified. False otherwise.
 */
bool modify_pointer_in_hash_tree(HashTree *tree, uint64_t hash, void *ptr);

/**
 * Query a binary tree for a pointer associated with a hash.
 *
 * @param  tree The tree to query
 * @param  hash The hash to find
 * @return      NULL if the associated pointer is null or if the node does not exist. A different pointer otherwise.
 */
void *get_pointer_from_hash_tree(HashTree *tree, const uint64_t hash);

/**
 * Query a tree for a Hash
 * Uses binary search, for a tree traversal, O(log_2 n).
 *
 * @param  tree The tree to query
 * @param  hash The hash to find
 * @return      True if it is present
 */
bool query_hash_tree(HashTree *tree, const uint64_t hash);

#endif  // __HASHTREE_H__

