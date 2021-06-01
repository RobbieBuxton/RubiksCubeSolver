#include "translate.h"

StatusCode sdt_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output, AssemblyInfo *assemblyInfo) {
    // The final instruction output.
    uint out = 0u;

    // Set instruction to be unconditionally executed.
    // al = 1110
    out |= FLAG_N | FLAG_Z | FLAG_C;

    // Set bits 27,26 to 01
    out |= 1 << 25u;

    // Check if this is an "ldr" or "str" instruction.
    if (tokens[0][0] == 'l') {
        out |= BIT_L;
    }

    // Collect Rd
    uint temp = (uint) strtoul(tokens[1], NULL, 10);
    out |= temp << 11u;

    // check_parse_error sets output to null if there is an error
    if (check_parse_error(output)) {
        return PARSE_ERROR;
    }

    if (temp > GENERAL_REGISTER_MAX) {
        *output = 0u;
        return INVALID_REGISTER;
    }

    // If the address is of the form <=expression>
    if (tokens[2][0] == '=') {
        temp = (uint) strtoul(tokens[2], NULL, 16);
        if (temp < 256u) {
            // return equivalent mov instruction binary
        }
        // Place value at end of assembly and calculate offset to there from PC
        uint offset = assemblyInfo->instructions - (current_offset + 2);
        offset <<= 2u;
        out |= offset;
        out |= PC << 15u;
        out |= BIT_P | BIT_U;
        *output = out;
        return CONTINUE;
    }

    return CONTINUE;

}