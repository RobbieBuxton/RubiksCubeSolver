#include "translate.h"

#include <string.h>

StatusCode sdt_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output, AssemblyInfo *assemblyInfo) {
    // The final instruction output.
    uint out = 0u;

    // Set instruction to be unconditionally executed.
    // al = 1110
    out |= FLAG_N | FLAG_Z | FLAG_C;

    // Set bits 27,26 to 01
    out |= 1 << 26u;

    // Check if this is an "ldr" or "str" instruction.
    if (tokens[0][0] == 'l') {
        out |= BIT_L;
    }

    // Collect Rd
    uint temp = (uint) strtoul(tokens[1] + 1, NULL, 10);
    out |= temp << 12u;

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
        temp = (uint) strtoul(tokens[2] + 1, NULL, 0);

        if (check_parse_error(output)) {
            return PARSE_ERROR;
        }

        if (temp < 256u) {
            // output = unconditional, mov, I = 1, S = 0, Rd = Rd from this, op2 = temp
            *output = 3642 << 20 | (out & (15 << 12)) | temp;
        } else {
            // Instruct caller function to place value in correct position
            assemblyInfo->int_to_load = true;
            assemblyInfo->load_int = temp;

            // Calculate offset to value from PC
            uint offset = assemblyInfo->instructions - (current_offset + 2);
            offset <<= 2u;
            out |= offset;
            out |= PC << 16u;
            out |= BIT_P | BIT_U;
            *output = out;
        }

        return CONTINUE;
    }

    // Collect Rn and store in output
    temp = (uint) strtoul(tokens[2] + 2, NULL, 10);

    if (check_parse_error(output)) {
        return PARSE_ERROR;
    }

    out |= temp << 16u;

    // If Rn was last token then it is pre-indexing
    if (!(tokens[3])) {
        out |= BIT_P | BIT_U;
        *output = out;
        return CONTINUE;
    }

    if (strchr(tokens[3], 'r')) {
        // Offset is a shifted register, BIT_U is decided by +/- prefix
        out |= BIT_I;
        // Collect BIT_U and Rm
        switch (tokens[3][0]) {
            case'+':
                out |= BIT_U;
            case '-':
                out |= (uint) strtol(tokens[3] + 2, NULL, 10);
                break;
            default:
                out |= BIT_U;
                out |= (uint) strtol(tokens[3] + 1, NULL, 10);
                break;
        }

        if (check_parse_error(output)) {
            return PARSE_ERROR;
        }

        // If there is a shift.
        if (tokens[4]) {
            // Collect shift type.
            if (strcmp(tokens[4], "lsl") == 0) {
                out |= 0u;
            } else if (strcmp(tokens[4], "lsr") == 0) {
                out |= 1u << 5u;
            } else if (strcmp(tokens[4], "asr") == 0) {
                out |= 2u << 5u;
            } else if (strcmp(tokens[4], "ror") == 0) {
                out |= 3u << 5u;
            }

            // Collect shift value/register.
            if (strchr(tokens[5], 'r')) {
                out |= (uint) strtol(tokens[5] + 1, NULL, 10) << 8u;
                out |= 1u << 4u;
            } else {
                out |= (uint) strtol(tokens[5] + 1, NULL, 0) << 7u;
            }

            if (check_parse_error(output)) {
                return PARSE_ERROR;
            }
        }
    } else {
        // Offset is immediate, BIT_U is decided by its sign
        sint tempSigned = (sint) strtol(tokens[3] + 1, NULL, 0);

        if (check_parse_error(output)) {
            return PARSE_ERROR;
        }

        out |= (tempSigned < 0) ? -tempSigned : (tempSigned | BIT_U);
    }

    // BIT_P is decided by where ] is
    if (!(strchr(tokens[2], ']'))) {
        out |= BIT_P;
    }

    *output = out;
    return CONTINUE;
}
