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
    uint temp = (uint) strtoul(&tokens[1][1], NULL, 10);
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
        temp = (uint) strtoul(&tokens[2][1], NULL, 0);
        if (check_parse_error(output)) {
            return PARSE_ERROR;
        }
        if (temp < 256u) {
            // return equivalent mov instruction binary
        }
        // Instruct caller function to place value in correct position
        assemblyInfo->int_to_load = true;
        assemblyInfo->load_int = temp;
        
        // Calculate offset to value from PC
        uint offset = assemblyInfo->instructions - (current_offset + 2);
        offset <<= 2u;
        out |= offset;
        out |= PC << 15u;
        out |= BIT_P | BIT_U;
        *output = out;
        return CONTINUE;
    }

    // Collect Rn and store in output
    temp = (uint) strtoul(&tokens[2][2], NULL, 10);
    if (check_parse_error(output)) {
        return PARSE_ERROR;
    }
    out |= temp << 15u;

    // If next token is of form [Rn]
    if (strstr(tokens[2], "[") && strstr(tokens[2], "]")) {
        // If there is a 4th token
        if (tokens[3]) {
            // post-indexing logic
            return CONTINUE;
        } else {
            out |= BIT_P | BIT_U;
            *output = out;
            return CONTINUE;
        }
    }

    // From here all alternatives are pre-indexing
    out |= BIT_P;

    if (strstr(tokens[3], "r")) {
        out |= BIT_I;
    } else {
        // Offset is immediate, BIT_U is decided by its sign
        sint tempSigned = (sint) strtol(&tokens[3][1], NULL, 0);
        out |= (tempSigned < 0) ? -tempSigned : (tempSigned | BIT_U);
    }

    return CONTINUE;

}