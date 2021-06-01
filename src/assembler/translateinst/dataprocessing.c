#include "translate.h"


#include <errno.h>
#include <helpers.h>


int parse_register(char *register_label) {
    return atoi(register_label[1]);
}

uint parse_operand2(char *operand_string){
    uint operand = strtol(operand_string[1], NULL, 0);
    uint shift = 0;
    do
    {
        if (operand & ~255)
        {
            operand = operand << 2;
            shift ++;
        } else {
            return operand | (shift << 8);
        }
    } while (shift < 16);
    //TODO: Give an error here (operand can't be represented).
    //TODO (optional): Add support shifted registers.
}

StatusCode dp_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output, AssemblyInfo assemblyInfo) {
    // The final instruction output.
    uint out = 0u;

    // Set instruction to be unconditionally executed.
    // al = 1110
    out |= FLAG_N | FLAG_Z | FLAG_C;

    SymbolMap *opcodes = new_symbol_map(10);
    add_to_symbol_map(opcodes, "and", dp_and);
    add_to_symbol_map(opcodes, "eor", dp_eor);
    add_to_symbol_map(opcodes, "sub", dp_sub);
    add_to_symbol_map(opcodes, "rsb", dp_rsb);
    add_to_symbol_map(opcodes, "add", dp_add);
    add_to_symbol_map(opcodes, "orr", dp_orr);
    add_to_symbol_map(opcodes, "mov", dp_mov);
    add_to_symbol_map(opcodes, "tst", dp_tst);
    add_to_symbol_map(opcodes, "teq", dp_teq);
    add_to_symbol_map(opcodes, "cmp", dp_cmp);

    //get the opcode and write it to the binary instruction
    DPOpCode opcode = query_symbol_map(opcodes, tokens[0]).addr;
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
        out |= parse_operand2(tokens[2]);
    default:
        out |= parse_register(tokens[1]) << 12u;
        out |= parse_register(tokens[2]) << 16u;
        out |= parse_operand2(tokens[3]);
        break;
    }

    free_symbol_map(opcodes);

    *output = out;
}