#include "translate.h"

#include "../mainmap.h"

#include <errno.h>
#include <string.h>

int parse_register(char *register_label) {
    return strtoul(register_label + 1, NULL, 10);
}

StatusCode parse_operand2(char **tokens, uint *output){
    if (!(strstr(tokens[0], "r"))) {
        uint operand = strtoul(tokens[0] + 1, NULL, 0);
        uint shift = 0;

        do {
            if (operand & ~255)
            {
                operand = operand << 2 | operand >> 30u;
                shift ++;
            } else {
                *output |= operand | (shift << 8);
                return CONTINUE;
            }
        } while (shift < 16);

        return PARSE_ERROR;
    }

    // Operand2 is a shifted register
    // Collect Rm
    *output |= parse_register(tokens[0]);
    if (check_parse_error(output)) {
        return PARSE_ERROR;
    }

    // If there is a shift.
    if (tokens[1]) {
        // Collect shift type.
        if (strcmp(tokens[1], "lsl") == 0) {
            *output |= 0u;
        } else if (strcmp(tokens[1], "lsr") == 0) {
            *output |= 1u << 5u;
        } else if (strcmp(tokens[1], "asr") == 0) {
            *output |= 2u << 5u;
        } else if (strcmp(tokens[1], "ror") == 0) {
            *output |= 3u << 5u;
        }

        // Collect shift value/register.
        if (strchr(tokens[2], 'r')) {
            *output |= parse_register(tokens[2]) << 8u;
            *output |= 1u << 4u;
        } else {
            *output |= (uint) strtoul(tokens[2] + 1, NULL, 0) << 7u;
        }

        if (check_parse_error(output)) {
            return PARSE_ERROR;
        }
    }
    return CONTINUE;
}

StatusCode dp_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output, AssemblyInfo *assemblyInfo) {
    // The final instruction output.
    uint out = 0u;

    // Set instruction to be unconditionally executed.
    // al = 1110
    out |= FLAG_N | FLAG_Z | FLAG_C;

    // Get the opcode
    DPOpCode opcode = query_symbol_map(translation_map, tokens[0]).addr;
    out |= opcode << 21u;

    // Set the I bit if immediate operands used
    for (char **t = tokens; t < tokens + 4; ++t) {
        if (*t && strchr(*t, '#')) {
            out |= BIT_I;
            break;
        }
    }

    // Set the operands
    switch (opcode){
        case dp_andeq:
            *output = 0u;
            return CONTINUE;
        case dp_lsl:
            opcode = dp_mov;
            out = FLAG_N | FLAG_Z | FLAG_C | opcode << 21u;
            out |= parse_register(tokens[1]) << 12u;
            if (out & BIT_I) {
                out -= 1u << 25u;
            }
            out |= strtoul(tokens[2] + 1, NULL, 0) << 7u;
            out |= parse_register(tokens[1]);
            break;
        case dp_tst:
        case dp_teq:
        case dp_cmp:
            // Set the S bit
            out |= 1u << 20u;
            out |= parse_register(tokens[1]) << 16u;
            if (parse_operand2(tokens + 2, &out)) return PARSE_ERROR;
            break;
        case dp_mov:
            out |= parse_register(tokens[1]) << 12u;
            if (parse_operand2(tokens + 2, &out)) return PARSE_ERROR;
            break;
        default:
            // and, eor, orr, add, sub, rsb
            out |= parse_register(tokens[1]) << 12u;
            out |= parse_register(tokens[2]) << 16u;
            if (parse_operand2(tokens + 3, &out)) return PARSE_ERROR;
            break;
    }

    *output = out;

    return CONTINUE;
}

