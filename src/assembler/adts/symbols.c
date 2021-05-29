#include "symbols.h"

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
    map->arr  = calloc(initial_size, sizeof(Symbol));

    if (!(map->arr)) {
        // If calloc fails, map->arr is null. Free the map so no consequences occur.
        free(map);

        return NULL;
    } else {
        // calloc successful, return newly made map
        return map;
    }
}

size_t extend_symbol_map(SymbolMap *map) {
    size_t new_size = map->size + (map->size >> 1u);

    // Attempt realloc of interal array.
    Symbol *new_arr = (Symbol *) realloc(map->arr, new_size);

    if (!new_arr) {
        // Realloc fails when it returns NULL.
        return 0u;
    } else {
        // Successful, assign new size and new array pointer.
        map->size = new_size;
        map->arr = new_arr;

        return new_size;
    }
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

bool add_to_symbol_map(SymbolMap *map, const char *symbol, const uint addr) {
    // Extend map if it is full.
    if (map->count >= map->size) {
        if (!extend_symbol_map(map)) {
            return false;
        }
    }

    // Add the symbol at count, as that always points to the next free location.
    strncpy(map->arr[map->count].name, symbol, MAXIMUM_SYMBOL_LENGTH);
    map->arr[map->count].addr = addr;

    // Increment the count.
    ++(map->count);

    return true;
}

void query_symbol_map(QueryResult *out_result, const SymbolMap *map, const char *symbol_name) {
    for (size_t i = 0; i < map->count; ++i) {
        Symbol *sym = map->arr + i;

        if (symbol_equals(sym, symbol_name)) {
            // Found! Set results as necessary.
            out_result->found = true;
            out_result->addr  = sym->addr;

            return;
        }
    }

    // Not found!
    out_result->found = false;
    out_result->addr  = 0u;
}

