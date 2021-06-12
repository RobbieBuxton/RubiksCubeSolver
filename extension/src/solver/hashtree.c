#include "hashtree.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

HashTree *new_hash_tree(void) {
    // Attempt to create a map.
    HashTree *map = (HashTree *) malloc(sizeof(HashTree));
    if (!map) {
        return NULL;
    }

    // Populate the map's fields.
    map->count = 0;
    map->root  = NULL;

    return map;
}

// Empty out a node.
static void clear_node(MapNode *symbol) {
    symbol->colour      = RED_NODE;
    symbol->hash        = 0u;
    symbol->parent      = NULL;
    symbol->left_child  = NULL;
    symbol->right_child = NULL;
}

static MapNode *new_node(const uint64_t hash) {
    MapNode *symbol = (MapNode *) malloc(sizeof(MapNode));
    if (!symbol) {
        // Calloc failed.
        return NULL;
    }

    // Initialise fields.
    clear_node(symbol);

    symbol->hash = hash;

    return symbol;
}

static bool free_node(MapNode *symbol) {
    if (!symbol) {
        return false;
    }

    // First, free the children:
    if (symbol->left_child) {
        assert(free_node(symbol->left_child));
    }

    if (symbol->right_child) {
        assert(free_node(symbol->right_child));
    }

    // Then, free the given node.
    free(symbol);

    return true;
}

bool free_hash_tree(HashTree *map) {
    if (!map) {
        return false;
    }

    // First, free the nodes of the tree
    if (map->root && !free_node(map->root)) {
        fprintf(stderr, "Warning: symbols from map at addr %zu failed to be freed!\n", (size_t) map);
    }

    // Then free the map itself.
    free(map);

    return true;
}

// Is this node a left child?
static bool is_left_child(MapNode *node) {
    return node->parent && ((node->parent)->left_child == node);
}

// Is this node a right child?
static bool is_right_child(MapNode *node) {
    return node->parent && ((node->parent)->right_child == node);
}

// Reparent a node, and return the ejected node.
// If the reparenting fails for any reason, return NULL.
static MapNode *reparent(HashTree *map, MapNode *current_node, MapNode *new_node) {
    // Ejected node.
    MapNode *cur = current_node;
    MapNode *par = cur->parent;

    if (!par) {
        // At root.
        map->root = new_node;
        new_node->parent = NULL;
    } else if (is_left_child(current_node)) {
        // Current node is left child of parent.
        par->left_child = new_node;
        new_node->parent = par;
        cur->parent = NULL;
    } else if (is_right_child(current_node)) {
        // Current node is right child of parent.
        par->right_child = new_node;
        new_node->parent = par;
        cur->parent = NULL;
    } else {
        fprintf(stderr, "Reparenting failed! Node at %zu does not have node at %zu as parent.\n", (size_t) cur, (size_t) par);
        return NULL;
    }

    return cur;
}

// Pre: current has a right child.
static void rotate_left(HashTree *map, MapNode *current) {
    assert(current->right_child);

    MapNode *right = current->right_child;
    MapNode *rights_left_child = right->left_child;

    // Set current's right to right's leftt child:
    current->right_child = rights_left_child;

    // Reparent right.
    current = reparent(map, current, right);
    assert(current);

    // Set right's left child to current.
    right->left_child = current;
}

// Pre: current has a left child.
static void rotate_right(HashTree *map, MapNode *current) {
    assert(current->left_child);

    MapNode *left = current->left_child;
    MapNode *lefts_right_child = left->right_child;

    // Set current's left to left's right child:
    current->left_child = lefts_right_child;

    // Reparent left.
    current = reparent(map, current, left);
    assert(current);

    // Set left's right child to current.
    left->right_child = current;
}

// Cases for post-insertion rebalancing and fixing of the tree.
static void case_1(HashTree *map, MapNode *current);
static void case_2(HashTree *map, MapNode *current);
static void case_3(HashTree *map, MapNode *current);
static void case_4(HashTree *map, MapNode *current);
static void case_5(HashTree *map, MapNode *current);

static void case_1(HashTree *map, MapNode *current) {
    if (!current->parent) {
        current->colour = BLACK_NODE;
    } else {
        case_2(map, current);
    }
}

static void case_2(HashTree *map, MapNode *current) {
    assert(current->parent);

    if (current->parent->colour != BLACK_NODE) {
        case_3(map, current);
    }
}

static void case_3(HashTree *map, MapNode *current) {
    MapNode *par, *gpar, *unc;

    par = current->parent;
    assert(par);

    gpar = par->parent;
    assert(gpar);

    if (is_left_child(par)) {
        unc = gpar->right_child;
    } else if (is_right_child(par)) {
        unc = gpar->left_child;
    } else {
        fprintf(stderr, "[Case 3]: Error, parent is in an invalid state!\n");
        assert(false);
    }

    if (unc->colour == RED_NODE) {
        par->colour = BLACK_NODE;
        unc->colour = BLACK_NODE;
        gpar->colour = RED_NODE;

        case_1(map, gpar);
    } else {
        case_4(map, current);
    }
}

static void case_4(HashTree *map, MapNode *current) {
    MapNode *par = current->parent;
    assert(par);

    if (is_left_child(par) && is_right_child(current)) {
        // 4a
        rotate_left(map, par);
        case_5(map, par);
    } else if (is_right_child(par) && is_left_child(current)) {
        // 4b
        rotate_right(map, par);
        case_5(map, par);
    } else {
        case_5(map, current);
    }
}

static void case_5(HashTree *map, MapNode *current) {
    MapNode *par, *gpar;

    par = current->parent;
    assert(par);

    gpar = par->parent;
    assert(gpar);

    par->colour = BLACK_NODE;
    gpar->colour = RED_NODE;

    if (is_left_child(current)) {
        // 5a
        rotate_right(map, gpar);
    } else if (is_right_child(current)) {
        // 5b
        rotate_left(map, gpar);
    } else {
        fprintf(stderr, "[Case 5] Error, the tree is in an invalid state!\n");
        assert(false);
    }
}

bool add_to_hash_tree(HashTree *map, uint64_t hash) {
    if (hash == 0ul) {
        fprintf(stderr, "Warning, zero hash!\n");
        return false;
    }

    if (map->root) {
        MapNode *curr_ptr = map->root;
        MapNode *next = NULL;
        MapNode **where = NULL;

        while (curr_ptr) {
            // Traverse the tree until we find a place to add the node.
            if (hash > curr_ptr->hash) {
                next = curr_ptr->right_child;

                if (next) {
                    curr_ptr = next;
                } else {
                    where = &(curr_ptr->right_child);
                    curr_ptr = next;
                }
            } else if (hash < curr_ptr->hash) {
                next = curr_ptr->left_child;

                if (next) {
                    curr_ptr = next;
                } else {
                    where = &(curr_ptr->left_child);
                    curr_ptr = next;
                }
            } else {
                // Hash is already in tree.
                return false;
            }
        }

        // Insert and fix.
        *where = new_node(hash);
        if (!where) {
            return false;
        }

        case_1(map, *where);
    } else {
        // Insert and fix.
        map->root = new_node(hash);
        if (!map->root) {
            return false;
        }

        map->root->hash = hash;

        case_1(map, map->root);
    }

    // Increment the count.
    ++(map->count);

    return true;
}

bool query_hash_tree(const HashTree *map, const uint64_t hash) {

    // Search for a hash.
    MapNode *curr_ptr = map->root;

    while (curr_ptr) {
        if (hash > curr_ptr->hash) {
            curr_ptr = curr_ptr->right_child;
        } else if (hash < curr_ptr->hash) {
            curr_ptr = curr_ptr->left_child;
        } else {
            return true;
        }
    }

    // Not found.
    return false;
}

