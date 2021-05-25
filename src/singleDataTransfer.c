#include "insttypes.h"
#include "executefuncs.h"

StatusCode sdt_execute(State* state) {
    // Retrieve components of instruction
    uint bits_ipuasl = state->decoded.inst.sdt.bits_ipuasl;
    Register base_register = state->decoded.inst.sdt.Rn;
    Register dest_register = state->decoded.inst.sdt.Rd;
    Register shifted_register;

    // If I is set, Offset is a shifted register.
    // Else, it is a 12 bit unsigned offset
    if (bits_ipuasl & BIT_I) {
        // TODO: copy shifted register interpreting from DPExecute once it's been done

        // Base register cannot be the same as the shifted register in a post-indexing SDT.
        if (!(bits_ipuasl & BIT_P) && base_register == shifted_register) {
            return FAILURE;
        }
    } else {
        uint offset = state->decoded.inst.sdt.offset;
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
        state->memory[address] = state->registers[dest_register];
    } else {
        state->registers[dest_register] = state->memory[address];
    }

    // If post-indexing, update the base register accordingly
    if (!(bits_ipuasl & BIT_P)) {
        state->registers[base_register] = (bits_ipuasl & BIT_U) ? address + offset : address - offset;
    }

    return CONTINUE;
}