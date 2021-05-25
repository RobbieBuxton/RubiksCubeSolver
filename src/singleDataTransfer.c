#include "insttypes.h"
#include "executefuncs.h"
#include "helpers.h"

StatusCode sdt_execute(State* state) {
    // Retrieve components of instruction
    uint bits_ipuasl = state->decoded.inst.sdt.bits_ipuasl;
    Register base_register = state->decoded.inst.sdt.Rn;
    Register dest_register = state->decoded.inst.sdt.Rd;
    Register shifted_register;
    uint offset;

    // If I is set, Offset is a shifted register.
    // Else, it is a 12 bit unsigned offset
    if (bits_ipuasl & BIT_I) {
        // Rotates the bits 7-0 of the opcode by bits 11-8 times 2.
        uint rot = (instr->operand2 >> 8u) << 1u;
        uint imm = instr->operand2 & 255u;
        offset = (imm << rot) | (imm >> (11u - rot));

        // Base register cannot be the same as the shifted register in a post-indexing SDT.
        if (!(bits_ipuasl & BIT_P) && base_register == shifted_register) {
            return FAILURE;
        }
    } else {
        offset = state->decoded.inst.sdt.offset;
    }

    // Calculate memory address to be loaded from/stored to.
    uint address = state->registers[base_register];
    if (bits_ipuasl & BIT_P) {
        address = (bits_ipuasl & BIT_U) ? address + offset : address - offset;
    }

    // Make sure memory address is neither negative nor too large
    if (address >= MAX_MEMORY_LOCATION) {
        return FAILURE;
    }

    // complete memory transfer
    if (bits_ipuasl & BIT_L) {
        load_word(state, address, dest_register);
    } else {
        store_word(state, address, dest_register);
    }

    // If post-indexing, update the base register accordingly
    if (!(bits_ipuasl & BIT_P)) {
        state->registers[base_register] = (bits_ipuasl & BIT_U) ? address + offset : address - offset;
    }

    return CONTINUE;
}