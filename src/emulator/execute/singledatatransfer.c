#include "../../helpers/helpers.h"

#include "executefuncs.h"

StatusCode sdt_execute(State *state) {
    // Retrieve components of instruction
    uint bits_ipuasl = state->decoded.inst.sdt.bits_ipuasl;
    SDTInst *inst = &(state->decoded.inst.sdt);
    Register base_register = inst->Rn;
    Register dest_register = inst->Rd;
    uint offset;

    // If I is set, Offset is a shifted register.
    // Else, it is a 12 bit unsigned offset
    if (bits_ipuasl & BIT_I) {
        offset = state->registers[(inst->offset & 15u)];
        uint shift_info = inst->offset >> 4u;
        uint shift;

        // Base register cannot be the same as the shifted register in a post-indexing SDT.
        if (!(bits_ipuasl & BIT_P) && base_register == (inst->offset & 15u)) {
            return INVALID_INSTRUCTION;
        }

        // By the value of int 11-7 when bit 4 = 0
        if (!(shift_info & 1u)) {
            shift = shift_info >> 3u;
        // By the value of register 11-8 when bit 4 = 1 and bit 7 = 0.
        } else if ((shift_info & 1u) && !(shift_info & 8u)) {
            Register shift_by = shift_info >> 4u;

            if (shift_by == PC) {
                // throw tried to access PC error.
                return INVALID_INSTRUCTION;
            }

            shift = state->registers[shift_by] & 255u;
        } else {
            // Throw unsupported instruction error
            return INVALID_INSTRUCTION;
        }

        switch (select_bits(shift_info, 3u, 1u, true)) {
            // In order: logical left, logical right, arithimetic right, rotate right.
            case lsl:
                offset <<= shift;
                break;
            case lsr:
                offset >>= shift;
                break;
            case asr:
                offset = (uint) ((sint) offset >> shift);
                break;
            case ror:
                offset = (offset >> shift) | (offset << (sizeof(offset) * 8u - shift));
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

    state->last_access = address;

    // Make sure memory address is neither negative nor too large
    if (address >= MAX_MEMORY_LOCATION && (address < PIN_TYPES_0_9_ADDR || address > CLEAR_ADDR)) {
        return ILLEGAL_MEMORY_ACCESS;
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

