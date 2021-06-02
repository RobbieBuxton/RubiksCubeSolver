#include "translate.h"
#include "../mainmap.h"
#include <errno.h>

int parse_register(char *register_label) {
    return atoi(register_label + 1);
}

StatusCode parse_operand2(char *operand_string, uint output){
    uint operand = strtol(operand_string + 1, NULL, 0);
    uint shift = 0;
    do
    {
        if (operand & ~255)
        {
            operand = operand << 2;
            shift ++;
        } else {
            output |= operand | (shift << 8);
            return CONTINUE;
        }
    } while (shift < 16);
    return PARSE_ERROR;
    //TODO (optional): Add support shifted registers.
}

StatusCode dp_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output, AssemblyInfo *assemblyInfo) {
    // The final instruction output.
    uint out = 0u;

    // Set instruction to be unconditionally executed.
    // al = 1110
    out |= FLAG_N | FLAG_Z | FLAG_C;

    //get the opcode and write it to the binary instruction
    DPOpCode opcode = query_symbol_map(translation_map, tokens[0]).addr;
    out |= opcode << 21u;

    //Set the I bit (may have to modify if we add support for shifted registers)
    out |= 1u << 25u;

    //Set the operands
    switch (opcode){
    case dp_tst:
    case dp_teq:
    case dp_cmp:
        out |= 1u << 20u;   //Set the S bit
        out |= parse_register(tokens[1]) << 16u;
        break;
    case dp_mov:
        out |= parse_register(tokens[1]) << 12u;
        if (parse_operand2(tokens[2], out)) return PARSE_ERROR;
        break;
    default:
        out |= parse_register(tokens[1]) << 12u;
        out |= parse_register(tokens[2]) << 16u;
        if (parse_operand2(tokens[3], out)) return PARSE_ERROR;
        break;
    }

    *output = out;

    return CONTINUE;
}
