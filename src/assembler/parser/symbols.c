#include "symbols.h"

#include "../../helpers/helpers.h"

#include <stddef.h>
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
    map->size = initial_size;
    map->arr  = (Symbol *) calloc(initial_size, sizeof(Symbol));

    if (!(map->arr)) {
        // If calloc fails, map->arr is null. Free the map so no consequences occur.
        free(map);

        return NULL;
    } else {
        // calloc successful, return newly made map
        return map;
    }
}

/**
 * Extend the size of a map to new_size.
 * Failing to extend a map does not free the map.
 *
 * @param  map      Map to extend
 * @param  new_size The new proposed size of the map
 * @return          The new size if successful. Returns 0 otherwise.
 */
size_t extend_symbol_map_with_size(SymbolMap *map, size_t new_size) {
    // Attempt realloc of interal array.
    Symbol *new_arr = (Symbol *) realloc(map->arr, new_size * sizeof(Symbol));

    if (!new_arr) {
        // Realloc fails when it returns NULL.
        return 0u;
    } else {
        // Do some post-initialisation to zero out the new memory.
        for (size_t i = map->size; i < new_size; ++i) {
            new_arr[i].hash = 0ul;
            new_arr[i].addr = 0u;
            memset(new_arr[i].name, '\0', MAXIMUM_SYMBOL_LENGTH);
        }

        // Successful, assign new size and new array pointer.
        map->size = new_size;
        map->arr = new_arr;

        return new_size;
    }
}

/**
 * Extend the size of a map by 50%.
 * Failing to extend a map does not free the map.
 *
 * @param  map Map to extend
 * @return     The new size if successful. Returns 0 otherwise.
 */
size_t extend_symbol_map(SymbolMap *map) {
    size_t new_size = map->size + (map->size >> 1u);
    return extend_symbol_map_with_size(map, new_size);
}

bool free_symbol_map(SymbolMap *map) {
    if (!map) {
        return false;
    }

    // First, free the array in the map.
    free(map->arr);

    // Then free the map itself.
    free(map);

    return true;
}

size_t left_child(size_t curr) {
    return (curr << 1u) + 1u;
}

size_t right_child(size_t curr) {
    return left_child(curr) + 1u;
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
        Symbol *sym = map->arr + i;

        if (symbol_equals(sym, symbol_name)) {
            // Found! Set results as necessary.
            out_result.found = true;
            out_result.addr  = sym->addr;

            return out_result;
        }
    }

    // Not found!
    out_result.found = false;
    out_result.addr  = 0u;

    return out_result;
}

