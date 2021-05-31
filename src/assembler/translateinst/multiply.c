#include "translate.h"

#include <assert.h>
#include <errno.h>

StatusCode check_parse_error(uint *output) {
    if (errno) {
        perror("Invalid number, caused by: ");

        *output = 0;
        return INVALID_INSTRUCTION;
    }

    return CONTINUE;
}

StatusCode m_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output) {
    // Assert that this instruction can be loaded correctly.
    assert(current_offset < MAX_MEMORY_LOCATION);

    // The final instruction output.
    uint out = 0u;

    // Set instruction to be unconditionally executed.
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
    out |= ((uint) strtoul(tokens[1], NULL, 10)) << 16u;

    if (check_parse_error(output)) {
        return INVALID_INSTRUCTION;
    }

    out |= ((uint) strtoul(tokens[2], NULL, 10));

    if (check_parse_error(output)) {
        return INVALID_INSTRUCTION;
    }

    out |= ((uint) strtoul(tokens[3], NULL, 10)) << 8u;

    if (check_parse_error(output)) {
        return INVALID_INSTRUCTION;
    }

    if (out & BIT_A) {
        // Collect Rn
        out |= ((uint) strtoul(tokens[4], NULL, 10)) << 12u;
        
        if (check_parse_error(output)) {
            return INVALID_INSTRUCTION;
        }
    }

    // Everything is fine. Return the instruction.
    *output = out;
    return CONTINUE;
}
