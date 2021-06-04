#ifndef __SYMBOLS_H__
#define __SYMBOLS_H__

#include <stdbool.h>
#include <stdlib.h>

#include "../../helpers/insttypes.h"

// Macros for use in this file:
#define MAXIMUM_SYMBOL_LENGTH 512

/**
 * "Colours" for the Symbol nodes.
 * Used for balancing a symbol map.
 */
typedef enum {
    RED,  /**< Red colouring for a node. */
    BLACK /**< Black colouring for a node. */
} SymbolColour;

/**
 * A symbol-address associative pair.
 */
typedef struct Symbol_t {
    ulong hash;                       /**< Hash identifier of symbol. */
    char name[MAXIMUM_SYMBOL_LENGTH]; /**< Identifier of symbol. */
    uint addr;                        /**< Address it represents. */

    SymbolColour colour;              /**< Symbol colour for use in balancing. */
    struct Symbol_t *parent;          /**< Parent symbol for this symbol. */
    struct Symbol_t *left_child;      /**< Left child for this symbol. */
    struct Symbol_t *right_child;     /**< Right child for this symbol. */
} Symbol;

/**
 * Compares if a symbol and an identifier are the same.
 *
 * @param  s1   Pointer to symbol
 * @param  name Name to compare with
 * @return      If their identifiers match
 */
bool symbol_equals(const Symbol *s1, const char *name);

/**
 * An associative tree map of symbols to the addresses.
 */
typedef struct {
    size_t count; /**< Number of items currently stored in the tree. */
    Symbol *root; /**< Pointer to the root of the map's symbol tree. */
} SymbolMap;

/**
 * Allocate a new symbol map. This map must be freed later using free_symbol_map.
 *
 * @param  initial_size Initial size for the associative array
 * @return              If successful, the pointer to the new symbol map.
 */
SymbolMap *new_symbol_map(size_t initial_size);

/**
 * Free a symbol map created by new_symbol_map.
 *
 * @param  map The map to free
 * @return     Success in freeing the map.
 */
bool free_symbol_map(SymbolMap *map);

/**
 * Add a symbol (and its address) to a symbol map.
 * Attempts to order the items by their hashes. O(n) worst case.
 * Failing to add a symbol does not free the map.
 *
 * @param[out] map    Symbol map to add symbol to
 * @param[in]  symbol Identifier of symbol
 * @param[in]  addr   Address that the symbol points to
 * @return            True if symbol was sucessfully added. False otherwise.
 */
bool add_to_symbol_map(SymbolMap* map, const char *symbol, const uint addr);

/**
 * Result of querying a symbol map for a certain symbol.
 */
typedef struct {
    bool found; /**< Was the symbol found? */
    uint addr;  /**< What address does it represent? */
} QueryResult;

/**
 * Query a map for a symbol name, storing its result in the returned QueryResult.
 * Uses the symbol hashes, for a tree traversal, O(log_2 n).
 *
 * @param  map         The map to query
 * @param  symbol_name The symbol name to find
 * @return             Query result. Check the struct fields for details.
 */
QueryResult query_symbol_map(const SymbolMap *map, const char *symbol_name);

#endif  // __SYMBOLS_H__

