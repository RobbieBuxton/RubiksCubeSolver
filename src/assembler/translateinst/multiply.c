#include "translate.h"

#include <assert.h>
#include <errno.h>

StatusCode m_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output, AssemblyInfo *assemblyInfo) {
    // The final instruction output.
    uint out = 0u;

    // Set instruction to be unconditionally executed.
    // al = 1110
    out |= FLAG_N | FLAG_Z | FLAG_C;

    // Add the 0b1001 bit sequence.
    out |= (9u << 4u);

    // Check if this is an "ml" or "mla" instruction.
    // It is assumed that the S bit is always 0.
    if (tokens[0][2] == 'a') {
        out |= BIT_A;
    }

    // Assuming the only registers used are r0 to r12...
    // And the tokenisation splits the string into: { ml/mla, rD, rM, rS, rN }
    // Note: rN only exists for mla

    // Collect Rd, Rm and Rs
    uint temp = (uint) strtoul(tokens[1] + 1, NULL, 10);
    out |= temp << 16u;

    // check_parse_error sets output to null if there is an error
    if (check_parse_error(output)) {
        return PARSE_ERROR;
    }

    if (temp > GENERAL_REGISTER_MAX) {
        *output = 0u;
        return INVALID_REGISTER;
    }

    temp = (uint) strtoul(tokens[2] + 1, NULL, 10);
    out |= temp;

    if (check_parse_error(output)) {
        return PARSE_ERROR;
    }

    if (temp > GENERAL_REGISTER_MAX) {
        *output = 0u;
        return INVALID_REGISTER;
    }


    temp = (uint) strtoul(tokens[3] + 1, NULL, 10);
    out |= temp << 8u;

    if (check_parse_error(output)) {
        return PARSE_ERROR;
    }

    if (temp > GENERAL_REGISTER_MAX) {
        *output = 0u;
        return INVALID_REGISTER;
    }


    if (out & BIT_A) {
        // Collect Rn
        temp = (uint) strtoul(tokens[4] + 1, NULL, 10);
        out |= temp << 12u;

        if (check_parse_error(output)) {
            return PARSE_ERROR;
        }

        if (temp > GENERAL_REGISTER_MAX) {
            *output = 0u;
            return INVALID_REGISTER;
        }
    }

    // Everything is fine. Return the instruction.
    *output = out;
    return CONTINUE;
}
