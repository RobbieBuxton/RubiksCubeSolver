#include "insttypes.h"
#include "executefuncs.h"
#include "helpers.h"

StatusCode sdt_execute(State *state) {
    // Retrieve components of instruction
    uint bits_ipuasl = state->decoded.inst.sdt.bits_ipuasl;
    SDTInst *inst = &(state->decoded.inst.sdt);
    Register base_register = inst->Rn;
    Register dest_register = inst->Rd;
    Register shifted_register;
    uint offset;

    // If I is set, Offset is a shifted register.
    // Else, it is a 12 bit unsigned offset
    if (bits_ipuasl & BIT_I) {
        uint offset = machineState->registers[(inst->offset & 15)];
        uint shift_info = inst->offset >> 4u;
        uint shift;

        // Base register cannot be the same as the shifted register in a post-indexing SDT.
        if (!(bits_ipuasl & BIT_P) && base_register == (inst->offset & 15)) {
            return FAILURE;
        }

        // by the value of int 11-7 when bit 4 = 0
        if (!(shift_info & 1)) {
            shift = shift_info >> 3u;
        // by the value of register 11-8 when bit 4 = 1 and bit 7 = 0.
        } else if ((shift_info & 1) && !(shift_info & 8)) {
            Register shift_by = shift_info >> 4u;
            if (shift_by == PC) {
                // throw tried to access PC error.
                return FAILURE;
            }
            shift = machineState->registers[shift_by] & 15;
        } else {
            // throw unsupported instruction error
            return FAILURE;
        }

        switch (select_bits(shift_info, 3, 1, true)) {
            // in order: logical left, logical right, arithimetic right, rotate right.
            case 0:
                offset <<= shift;
                break;
            case 1:
                offset >>= shift;
                break;
            case 2:
                offset = uint ((int32_t) offset >> shift);
                break;
            case 3:
                offset = (offset >> shift) | (offset << (sizeof(offset) - shift))
                break;
            default:
                // should not reach this ever
                return FAILURE;
        }
    } else {
        offset = inst->offset;
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
