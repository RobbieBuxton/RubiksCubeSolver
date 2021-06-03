#include "translate.h"

#include "../mainmap.h"

#include <assert.h>
#include <errno.h>

StatusCode b_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output, AssemblyInfo *assemblyInfo) {
    // Just to make sure...
    *output = 0u;

    // Set xxxx1010....
    *output |= (1u << 27u) | (1u << 25u);

    // Set condition bits
    QueryResult cond_result = query_symbol_map(translation_map, tokens[0]);
    if (cond_result.found) {
        *output |= (cond_result.addr) << 28u;
    } else {
        *output = 0u;
        return INVALID_INSTRUCTION;
    }

    // Get current address, +2 due to pipeline
    uint current_address = current_offset + 2u;

    // Get target address
    uint target_address = 0u;
    QueryResult label_addr_result = query_symbol_map(symbols, tokens[1]);

    if (label_addr_result.found) {
        target_address = label_addr_result.addr / INSTRUCTION_BYTE_LENGTH;
    } else {
        *output = 0u;
        return SYMBOL_DOES_NOT_EXIST;
    }

    // Calculate offset
    sint offset = (sint) target_address - (sint) current_address;
    if (abs(offset) >= (1 << 24)) {
        return INVALID_INSTRUCTION;
    }

    // Add to instruction
    uint final_offset = ((uint) offset) & ((1u << 24u) - 1u);
    *output |= final_offset;

    return CONTINUE;
}

