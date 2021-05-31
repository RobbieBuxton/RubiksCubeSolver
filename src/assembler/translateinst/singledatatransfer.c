#include "translate.h"

StatusCode sdt_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output, AssemblyInfo assemblyInfo) {
    // The final instruction output.
    uint out = 0u;

    // Set instruction to be unconditionally executed.
    // al = 1110
    out |= FLAG_N | FLAG_Z | FLAG_C;

    // Set bits 27,26 to 01
    out |= 1 << 25u;

    // Check if this is an "ldr" or "str" instruction.
    // It is assumed that the S bit is always 0.
    if (tokens[0][0] == 'l') {
        out |= BIT_L;
    }



}