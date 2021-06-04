#include "symbols.h"

#include "../../helpers/helpers.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool symbol_equals(const Symbol *s1, const char *name) {
    return !(strcmp(s1->name, name));
}

SymbolMap *new_symbol_map(size_t initial_size) {
    // Attempt to create a map.
    SymbolMap *map = (SymbolMap *) malloc(sizeof(SymbolMap));
    if (!map) {
        return NULL;
    }

    // Populate the map's fields.
    map->count = 0;
    map->root  = NULL;

    return map;
}

// Empty out a node.
static void clear_node(Symbol *symbol) {
    symbol->colour      = RED;
    symbol->hash        = 0u;
    symbol->parent      = NULL;
    symbol->left_child  = NULL;
    symbol->right_child = NULL;

    memset(symbol->name, 0, MAXIMUM_SYMBOL_LENGTH);
    symbol->addr = 0u;
}

static Symbol *new_symbol(const char *label, const uint addr) {
    Symbol *symbol = (Symbol *) malloc(sizeof(Symbol));
    if (!symbol) {
        // Calloc failed.
        return NULL;
    }

    // Initialise fields.
    clear_node(symbol);

    strncpy(symbol->name, label, MAXIMUM_SYMBOL_LENGTH - 1);
    symbol->addr        = addr;

    return symbol;
}

static bool free_symbol(Symbol *symbol) {
    if (!symbol) {
        return false;
    }

    // First, free the children:
    if (symbol->left_child) {
        assert(free_symbol(symbol->left_child));
    }

    if (symbol->right_child) {
        assert(free_symbol(symbol->right_child));
    }

    // Then, free the given symbol.
    free(symbol);

    return true;
}

bool free_symbol_map(SymbolMap *map) {
    if (!map) {
        return false;
    }

    // First, free the symbols in the map.
    if (map->root && !free_symbol(map->root)) {
        fprintf(stderr, "Warning: symbols from map at addr %zu failed to be freed!\n", (size_t) map);
    }

    // Then free the map itself.
    free(map);

    return true;
}

// Is this node a left child?
static bool is_left_child(Symbol *node) {
    return node->parent && ((node->parent)->left_child == node);
}

// Is this node a right child?
static bool is_right_child(Symbol *node) {
    return node->parent && ((node->parent)->right_child == node);
}

// Reparent a node, and return the ejected node.
// If the reparenting fails for any reason, return NULL.
static Symbol *reparent(SymbolMap *map, Symbol *current_node, Symbol *new_node) {
    // Ejected node.
    Symbol *cur = current_node;
    Symbol *par = cur->parent;

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
static void rotate_left(SymbolMap *map, Symbol *current) {
    assert(current->right_child);

    Symbol *right = current->right_child;
    Symbol *rights_left_child = right->left_child;

    // Set current's right to right's leftt child:
    current->right_child = rights_left_child;

    // Reparent right.
    current = reparent(map, current, right);
    assert(current);

    // Set right's left child to current.
    right->left_child = current;
}

// Pre: current has a left child.
static void rotate_right(SymbolMap *map, Symbol *current) {
    assert(current->left_child);

    Symbol *left = current->left_child;
    Symbol *lefts_right_child = left->right_child;

    // Set current's left to left's right child:
    current->left_child = lefts_right_child;

    // Reparent left.
    current = reparent(map, current, left);
    assert(current);

    // Set left's right child to current.
    left->right_child = current;
}

// Cases for post-insertion rebalancing and fixing of the tree.
static void case_1(SymbolMap *map, Symbol *current);
static void case_2(SymbolMap *map, Symbol *current);
static void case_3(SymbolMap *map, Symbol *current);
static void case_4(SymbolMap *map, Symbol *current);
static void case_5(SymbolMap *map, Symbol *current);

static void case_1(SymbolMap *map, Symbol *current) {
    if (!current->parent) {
        current->colour = BLACK;
    } else {
        case_2(map, current);
    }
}

static void case_2(SymbolMap *map, Symbol *current) {
    assert(current->parent);

    if (current->parent->colour != BLACK) {
        case_3(map, current);
    }
}

static void case_3(SymbolMap *map, Symbol *current) {
    Symbol *par, *gpar, *unc;

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

    if (unc->colour == RED) {
        par->colour = BLACK;
        unc->colour = BLACK;
        gpar->colour = RED;

        case_1(map, gpar);
    } else {
        case_4(map, current);
    }
}

static void case_4(SymbolMap *map, Symbol *current) {
    Symbol *par = current->parent;
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

static void case_5(SymbolMap *map, Symbol *current) {
    Symbol *par, *gpar;

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

bool add_to_symbol_map(SymbolMap *map, const char *symbol, const uint addr) {
    // Extend map if it is full.
    if (map->count >= map->size) {
        if (!extend_symbol_map(map)) {
            return false;
        }
    }

    // Add symbol by its hash, while trying to maintain order in the hashes.
    ulong symbol_hash = hash_string(symbol);
    size_t curr_ptr = 0u;

    do {
        if (curr_ptr >= map->size) {
            // We've exited the map. Extend it to fit.
            if (!extend_symbol_map_with_size(map, curr_ptr + 1)) {
                return false;
            }
        }

        ulong ptr_hash = map->arr[curr_ptr].hash;

        // Found empty child. Set all fields.
        if (!ptr_hash) {
            strncpy(map->arr[curr_ptr].name, symbol, MAXIMUM_SYMBOL_LENGTH);
            map->arr[curr_ptr].hash = symbol_hash;
            map->arr[curr_ptr].addr = addr;
            break;
        }

        if (symbol_hash > ptr_hash) {
            curr_ptr = right_child(curr_ptr);
        } else {
            curr_ptr = left_child(curr_ptr);
        }
    } while (true);

    // Increment the count.
    ++(map->count);

    return true;
}

QueryResult query_symbol_map(const SymbolMap *map, const char *symbol_name) {
    // Storage of search result.
    QueryResult out_result;

    // Search for symbol by its hash. If a found, use strcmp to compare first.
    ulong symbol_hash = hash_string(symbol_name);
    size_t curr_ptr = 0u;

    do {
        // We've exited the map. Exit.
        if (curr_ptr >= map->size) {
            break;
        }

        // Get current pointer hash.
        ulong ptr_hash = map->arr[curr_ptr].hash;

        // Reached empty node. Exit.
        if (!ptr_hash) {
            break;
        }

        if (symbol_hash > ptr_hash) {
            curr_ptr = right_child(curr_ptr);
        } else if (symbol_hash < ptr_hash) {
            curr_ptr = left_child(curr_ptr);
        } else {
            if (strcmp(symbol_name, map->arr[curr_ptr].name) == 0) {
                out_result.found = true;
                out_result.addr  = map->arr[curr_ptr].addr;

                return out_result;
            } else {
                curr_ptr = left_child(curr_ptr);
            }
        }
    } while (true);

    // Not found!
    out_result.found = false;
    out_result.addr  = 0u;

    return out_result;
}

QueryResult safe_query_symbol_map(const SymbolMap *map, const char *symbol_name) {
    // Storage of search result.
    QueryResult out_result;

    for (size_t i = 0; i < map->count; ++i) {
        Symbol *symbol = map->arr + i;

        if (symbol_equals(symbol, symbol_name)) {
            // Found! Set results as necessary.
            out_result.found = true;
            out_result.addr  = symbol->addr;

            return out_result;
        }
    }

    // Not found!
    out_result.found = false;
    out_result.addr  = 0u;

    return out_result;
}

