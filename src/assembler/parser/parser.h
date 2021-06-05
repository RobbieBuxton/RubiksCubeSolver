#ifndef __PARSER_H__
#define __PARSER_H__

// Include the symbol association array.
#include "symbols.h"

// For FILE and size_t
#include <stdio.h>
#include <stdlib.h>

// Macros for use in parser:
#define MAXIMUM_LINE_LENGTH 512

/**
 * Stores basic information about the contents of an assembly file.
 */
typedef struct {
    size_t instructions; /**< The number of instructions in the file */
    size_t symbols;      /**< The number of symbols in the file */
    uint load_int;       /**< Used whilst encoding load instructions */
    bool int_to_load;    /**< Flag informing assembler whether load_int should be added */
} AssemblyInfo;

/**
 * First pass of the two-pass assembly, collecting the symbols in the file.
 *
 * @param[out] map  Symbol map to update
 * @param[in]  file Pointer to open assembly file (expected mode: "r")
 * @return          Instruction and symbol count for file
 */
AssemblyInfo collect_symbols(StringUintMap *map, FILE *file);

#endif  // __PARSER_H__

