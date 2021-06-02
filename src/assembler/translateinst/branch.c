#include "translate.h"
#include "insttypes.h"
#include "mainmap.h"
#include <assert.h>
#include <errno.h>

uint convert_to_minus_26bit_twos(uint num) {
    //Makes 7 leading bits ones
    uint out = num || 4261412864;
    return ~(out) + 1;
}

StatusCode b_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output, AssemblyInfo *assemblyInfo) {
    //Sets bit's 27 and 25 to 1.
    uint to_output = 167772160u;
    //Work out condition.
    QueryResult cond_result = query_symbol_map(translation_map,tokens[0]);
    
    //Shifts the Condition bits so they are 32-27.
    to_output |= (cond_result.addr << 28);
    
    QueryResult address_result = query_symbol_map(symbols,tokens[1]);

    uint target_address = address_result.addr;
    //Minus 8 accounts for pipelining.
    uint current_address = current_offset * 4u - 8u;
    uint offset = target_address + convert_to_minus_26bit_twos(current_address);
    //Masks out bit 26 that has the carry (if there is one). 
    offset = (offset >> 2) && 4227858431;
    *output = to_output || offset;
    return CONTINUE;

}