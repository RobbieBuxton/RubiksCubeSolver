#include "symbols.h"

#include "../../helpers/helpers.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Compares if a symbol and an identifier are the same.
 *
 * @param  s1   Pointer to symbol
 * @param  name Name to compare with
 * @return      If their identifiers match, a pair is returned.
 */
static __StringUintPair__ *symbol_equals(const MapNode *sym, const char *name) {
    for (size_t i = 0; i < sym->amount; ++i) {
        if (!strcmp(sym->pairs[i].string, name)) {
            return sym->pairs + i;
        }
    }

    return NULL;
}

StringUintMap *new_string_uint_map(void) {
    // Attempt to create a map.
    StringUintMap *map = (StringUintMap *) malloc(sizeof(StringUintMap));
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
    symbol->colour      = RED;
    symbol->hash        = 0u;
    symbol->parent      = NULL;
    symbol->left_child  = NULL;
    symbol->right_child = NULL;
    symbol->pairs       = NULL;
    symbol->amount      = 0u;
}

static MapNode *new_node(const char *label, const ulong hash, const uint addr) {
    MapNode *symbol = (MapNode *) malloc(sizeof(MapNode));
    if (!symbol) {
        // Calloc failed.
        return NULL;
    }

    // Initialise fields.
    clear_node(symbol);

    symbol->amount = 1u;
    symbol->pairs = (__StringUintPair__ *) malloc(sizeof(__StringUintPair__));

    if (symbol->pairs) {
        symbol->hash = hash;

        strncpy(symbol->pairs[0].string, label, MAXIMUM_SYMBOL_LENGTH - 1);
        symbol->pairs[0].value = addr;

        return symbol;
    } else {
        free(symbol);
        return NULL;
    }
}

static MapNode *modify_or_append_pair(MapNode *symbol, const char *label, const uint addr) {
    // Get old size.
    size_t old_amt = (symbol->amount)++;

    // If there is already the same label, modify it instead of appending a new one.
    __StringUintPair__ *found = symbol_equals(symbol, label);
    if (found) {
        found->value = addr;
        return symbol;
    }

    __StringUintPair__ *new_arr = realloc(symbol->pairs, symbol->amount * sizeof(__StringUintPair__));

    if (!new_arr) {
        // Failed to reallocate.
        return NULL;
    }

    strncpy(symbol->pairs[old_amt].string, label, MAXIMUM_SYMBOL_LENGTH - 1);
    symbol->pairs[old_amt].value = addr;

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
    free(symbol->pairs);
    free(symbol);

    return true;
}

bool free_string_uint_map(StringUintMap *map) {
    if (!map) {
        return false;
    }

    // First, free the symbols in the map.
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
static MapNode *reparent(StringUintMap *map, MapNode *current_node, MapNode *new_node) {
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
static void rotate_left(StringUintMap *map, MapNode *current) {
    assert(current->right_child);

    MapNode *right = current->right_child;
    MapNode *rights_left_child = right->left_child;

    // Set current's right to right's leftt child:
    current->right_child = rights_left_child;
    if (rights_left_child) {
        rights_left_child->parent = current;
    }

    // Reparent right.
    current = reparent(map, current, right);
    assert(current);

    // Set right's left child to current.
    right->left_child = current;
    current->parent = right;
}

// Pre: current has a left child.
static void rotate_right(StringUintMap *map, MapNode *current) {
    assert(current->left_child);

    MapNode *left = current->left_child;
    MapNode *lefts_right_child = left->right_child;

    // Set current's left to left's right child:
    current->left_child = lefts_right_child;
    if (lefts_right_child) {
        lefts_right_child->parent = current;
    }

    // Reparent left.
    current = reparent(map, current, left);
    assert(current);

    // Set left's right child to current.
    left->right_child = current;
    current->parent = left;
}

// Cases for post-insertion rebalancing and fixing of the tree.
static void case_1(StringUintMap *map, MapNode *current);
static void case_2(StringUintMap *map, MapNode *current);
static void case_3(StringUintMap *map, MapNode *current);
static void case_4(StringUintMap *map, MapNode *current);
static void case_5(StringUintMap *map, MapNode *current);

static void case_1(StringUintMap *map, MapNode *current) {
    if (!current->parent) {
        current->colour = BLACK;
    } else {
        case_2(map, current);
    }
}

static void case_2(StringUintMap *map, MapNode *current) {
    assert(current->parent);

    if (current->parent->colour != BLACK) {
        case_3(map, current);
    }
}

static void case_3(StringUintMap *map, MapNode *current) {
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

    if (unc && unc->colour == RED) {
        par->colour = BLACK;
        unc->colour = BLACK;
        gpar->colour = RED;

        case_1(map, gpar);
    } else {
        case_4(map, current);
    }
}

static void case_4(StringUintMap *map, MapNode *current) {
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

static void case_5(StringUintMap *map, MapNode *current) {
    MapNode *par, *gpar;

    par = current->parent;
    assert(par);

    gpar = par->parent;
    assert(gpar);

    par->colour = BLACK;
    gpar->colour = RED;

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

bool add_to_string_uint_map(StringUintMap *map, const char *symbol, const uint addr) {
    // Add symbol by its hash, while trying to maintain order in the hashes.
    ulong symbol_hash = hash_string(symbol);
    if (symbol_hash == 0ul) {
        fprintf(stderr, "Warning, string '%s' has a zero hash!\n", symbol);
        return false;
    }

    if (map->root) {
        MapNode *curr_ptr = map->root;
        MapNode *next = NULL;
        MapNode *parent = NULL;
        MapNode **where = NULL;

        while (curr_ptr) {
            // Traverse the tree until we find a place to add the node.
            if (symbol_hash > curr_ptr->hash) {
                next = curr_ptr->right_child;

                if (next) {
                    curr_ptr = next;
                } else {
                    where = &(curr_ptr->right_child);
                    parent = curr_ptr;
                    curr_ptr = next;
                }
            } else if (symbol_hash < curr_ptr->hash) {
                next = curr_ptr->left_child;

                if (next) {
                    curr_ptr = next;
                } else {
                    where = &(curr_ptr->left_child);
                    parent = curr_ptr;
                    curr_ptr = next;
                }
            } else {
                // Modify / append instead.
                if (!modify_or_append_pair(curr_ptr, symbol, addr)) {
                    fprintf(stderr, "Failed to reallocate pairs in node %zu.\n", (size_t) curr_ptr);
                }

                // We do not need to rearrange the tree.
                goto inc_map;
            }
        }

        // Insert and fix.
        *where = new_node(symbol, symbol_hash, addr);
        if (!*where) {
            return false;
        }

        (*where)->parent = parent;
        case_1(map, *where);
    } else {
        // Insert and fix.
        map->root = new_node(symbol, symbol_hash, addr);
        if (!map->root) {
            return false;
        }

        case_1(map, map->root);
    }

inc_map:
    // Increment the count.
    ++(map->count);

    return true;
}

QueryResult query_string_uint_map(const StringUintMap *map, const char *symbol_name) {
    // Storage of search result.
    QueryResult out_result;

    // Search for symbol by its hash. If a found, use strcmp to compare first.
    ulong symbol_hash = hash_string(symbol_name);
    MapNode *curr_ptr = map->root;
    __StringUintPair__ *pair;

    while (curr_ptr) {
        if (symbol_hash > curr_ptr->hash) {
            curr_ptr = curr_ptr->right_child;
        } else if (symbol_hash < curr_ptr->hash) {
            curr_ptr = curr_ptr->left_child;
        } else {
            // Equal...?
            if ((pair = symbol_equals(curr_ptr, symbol_name))) {
                // All fine.
                out_result.found = true;
                out_result.addr  = pair->value;
                return out_result;
            }
        }
    }

    // Not found!
    out_result.found = false;
    out_result.addr  = 0u;

    return out_result;
}

