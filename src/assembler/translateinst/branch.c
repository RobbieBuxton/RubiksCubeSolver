#include "translate.h"
#include "../mainmap.h"
#include <assert.h>
#include <errno.h>

#ifndef __SHORTEN__
// Allow language servers to get completion info.
#include "../../helpers/insttypes.h"
#else
#include "insttypes.h"
#endif

uint convert_to_minus_26bit_twos(uint num) {
    //Makes 7 leading bits ones
    uint out = num | 4261412864;
    return ~(out) + 1;
}

StatusCode b_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output, AssemblyInfo *assemblyInfo) {
    // TODO: this
    /* //Sets bit's 27 and 25 to 1. */
    /* uint to_output = 167772160u; */
    /* //Work out condition. */
    /* QueryResult cond_result = query_symbol_map(translation_map,tokens[0]); */

    /* //Shifts the Condition bits so they are 32-27. */
    /* to_output |= (cond_result.addr << 28); */

    /* QueryResult address_result = query_symbol_map(symbols,tokens[1]); */

    /* uint target_address = address_result.addr; */
    /* //Minus 8 accounts for pipelining. */
    /* uint current_address = current_offset * 4u - 8u; */
    /* uint offset = target_address + convert_to_minus_26bit_twos(current_address); */
    /* //Masks out bit 26 that has the carry (if there is one). */
    /* offset = (offset >> 2) & 4227858431; */
    /* *output = to_output | offset; */
    /* return CONTINUE; */

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
        target_address = label_addr_result.addr / 4u;
    } else {
        *output = 0u;
        return SYMBOL_DOES_NOT_EXIST;
    }

    // Calculate offset
    sint offset = (sint) target_address - (sint) current_address;
    if (abs(offset) >= (1 << 25)) {
        return INVALID_INSTRUCTION;
    }

    // Add to instruction
    uint final_offset = ((uint) offset) & ((1u << 24u) - 1u);
    *output |= final_offset;

    return CONTINUE;
}
