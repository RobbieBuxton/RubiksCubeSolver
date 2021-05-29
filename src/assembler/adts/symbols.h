#ifndef __SYMBOLS_H__
#define __SYMBOLS_H__

#include <stdbool.h>
#include <stdlib.h>

#include "../../helpers/insttypes.h"

// Macros for use in parser:
#define MAXIMUM_LINE_LENGTH   512
#define MAXIMUM_SYMBOL_LENGTH 120

/**
 * The different mnemonics used in the assembly.
 */
typedef enum {
    asm_add,  /**< DP: add */
    asm_sub,  /**< DP: sub */
    asm_rsb,  /**< DP: reverse sub */
    asm_and,  /**< DP: logical AND */
    asm_eor,  /**< DP: logical XOR */
    asm_orr,  /**< DP: logical OR */
    asm_mov,  /**< DP: move to register */
    asm_tst,  /**< DP: test bits */
    asm_teq,  /**< DP: test bitwise equality */
    asm_cmp,  /**< DP: compare */
    asm_mul,  /**< M: multiply */
    asm_mla,  /**< M: multiply accumulate */
    asm_ldr,  /**< SDT: load register */
    asm_str,  /**< SDT: store register */
    asm_beq,  /**< B: branch if == */
    asm_bne,  /**< B: branch if != */
    asm_bge,  /**< B: branch if >= */
    asm_blt,  /**< B: branch if < */
    asm_bgt,  /**< B: branch if > */
    asm_ble,  /**< B: branch if <= */
    asm_b,    /**< B: unconditional branch */
    asm_lsl,  /**< SPECIAL: left shift */
    asm_andeq /**< SPECIAL: halt */
} Mnemonic;

/**
 * A symbol-address associative pair.
 */
typedef struct {
    char name[MAXIMUM_SYMBOL_LENGTH]; /**< Identifier of symbol. */
    uint addr;                        /**< Address it represents. */
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
 * An associative array of symbols to the addresses.
 */
typedef struct {
    size_t size;  /**< Size of associative array. */
    size_t count; /**< Number of items currently stored in array. */
    Symbol *arr;  /**< Pointer to start of associative array. */
} SymbolMap;

/**
 * Allocate a new symbol map. This map must be freed later using free_symbol_map.
 *
 * @param  initial_size Initial size for the associative array
 * @return              If successful, the pointer to the new symbol map.
 */
SymbolMap *new_symbol_map(size_t initial_size);

/**
 * Extend the size of a map by 50%.
 *
 * @param  map Map to extend
 * @return     The new size if successful. Returns 0 otherwise.
 */
size_t extend_symbol_map(SymbolMap *map);

/**
 * Free a symbol map created by new_symbol_map.
 *
 * @param  map The map to free
 * @return     Success in freeing the map.
 */
bool free_symbol_map(SymbolMap *map);

/**
 * Result of querying a symbol map for a certain symbol.
 */
typedef struct {
    bool found; /**< Was the symbol found? */
    uint addr;  /**< What address does it represent? */
} QueryResult;

/**
 * Query a map for a symbol name, storing its result in the given QueryStruct pointer.
 *
 * @param[out] out_result  The pointer to store the query result
 * @param[in]  map         The map to query
 * @param[in]  symbol_name The symbol name to find
 */
void query_symbol_map(QueryResult *out_result, const SymbolMap *map, const char *symbol_name);

#endif  // __SYMBOLS_H__

