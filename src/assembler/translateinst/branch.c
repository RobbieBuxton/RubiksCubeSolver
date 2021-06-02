#include "translate.h"
#include "insttypes.h"
#include <assert.h>
#include <errno.h>

StatusCode b_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output, AssemblyInfo *assemblyInfo) {
    //Sets bit's 27 and 25 to 1.
    output = 167772160u;
    //Work out condition.
    
    QueryResult address_result = query_symbol_map(symbols,tokens[1]);
    
    if (address_result.found = true) {
        (uint) output |= address_result.addr;
        return CONTINUE;
    }

    return SYMBOL_DOES_NOT_EXIST;
    



}