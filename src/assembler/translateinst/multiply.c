#include "translate.h"

#include "../mainmap.h"

#include <assert.h>
#include <errno.h>

StatusCode m_translate(char **tokens, StringUintMap *symbols, uint current_offset, uint *output, AssemblyInfo *assemblyInfo) {
    // The final instruction output.
    uint out = 0u;

    // Get the correct multiply instruction starter bits.
    // See insttypes.h for more information on the MulType enum.
    QueryResult mul_inst_query = query_string_uint_map(translation_map, tokens[0]);
    if (!mul_inst_query.found) {
        *output = 0u;
        return INVALID_INSTRUCTION;
    }

    // Field is named addr due to map's original use for collecting symbols from the assembly.
    out |= mul_inst_query.addr;

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

