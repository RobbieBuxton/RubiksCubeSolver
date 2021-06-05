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
} NodeColour;

/**
 * Internal struct used to keep pairs of labels to their addresses.
 */
typedef struct {
    char string[MAXIMUM_SYMBOL_LENGTH]; /**< Identifier of symbol. */
    uint value;                         /**< Address it represents. */
} __StringUintPair__;

/**
 * A symbol-address associative pair.
 */
typedef struct MapNode_t {
    ulong hash;                    /**< Hash identifier of symbols. */
    size_t amount;                 /**< Number of symbols with this hash. */
    __StringUintPair__ *pairs;     /**< Symbols with this hash. */

    NodeColour colour;             /**< Symbol colour for use in balancing. */
    struct MapNode_t *parent;      /**< Parent symbol for this symbol. */
    struct MapNode_t *left_child;  /**< Left child for this symbol. */
    struct MapNode_t *right_child; /**< Right child for this symbol. */
} MapNode;


/**
 * An associative tree map of symbols to the addresses.
 */
typedef struct {
    size_t count;  /**< Number of items currently stored in the tree. */
    MapNode *root; /**< Pointer to the root of the map's symbol tree. */
} StringUintMap;

/**
 * Allocate a new symbol map. This map must be freed later using free_symbol_map.
 *
 * @return              If successful, the pointer to the new symbol map.
 */
StringUintMap *new_string_uint_map(void);

/**
 * Free a symbol map created by new_string_uint_map.
 *
 * @param  map The map to free
 * @return     Success in freeing the map.
 */
bool free_string_uint_map(StringUintMap *map);

/**
 * Add a string (and its value) to a map.
 * Failing to add a symbol does not free the map.
 *
 * @param[out] map    Symbol map to add symbol to
 * @param[in]  symbol Identifier of symbol
 * @param[in]  addr   Address that the symbol points to
 * @return            True if symbol was sucessfully added. False otherwise.
 */
bool add_to_string_uint_map(StringUintMap* map, const char *symbol, const uint addr);

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
QueryResult query_string_uint_map(const StringUintMap *map, const char *symbol_name);

#endif  // __SYMBOLS_H__

