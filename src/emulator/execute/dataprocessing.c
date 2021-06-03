#include "executefuncs.h"

#include "../../helpers/helpers.h"

#include <stddef.h>

/**
 * Get the carry bit from a shifter operation.
 *
 * @param  num      Initial number
 * @param  amount   Shift amount
 * @param  bit_size Bit length of number
 * @param  type     Shift operation
 * @return          The caarry bit as a result from the operation
 */
uint get_carry_bit(uint num, uint amount, uint bit_size, ShiftType type);

/**
 * Select a range of bits from a number.
 *
 * @remark            Bits are 0 indexed from the right.
 *
 * @param  num        Initial number
 * @param  start_bit  Leftmost bit to select
 * @param  finish_bit Rightmost bit to select
 * @return            Numeric value of selected bits
 */
uint select_range(uint num, uint start_bit, uint finish_bit) {
    uint upper_mask;
    if (start_bit == 31u) {
        //MAX_UINT
        upper_mask = 0u - 1u;
    } else {
        upper_mask = ((1u << (start_bit + 1u)) - 1u);
    }
    uint lower_mask = ((1u << finish_bit) - 1u);
    return (uint) ((upper_mask ^ lower_mask) & num) >> finish_bit;
}

/**
 * The barrel shifter for a data processing instruction.
 *
 * @param  num      Initial number
 * @param  amount   Amount to shift
 * @param  bit_size Bit size of number
 * @param  type     Shifter operation
 * @param  C        Pointer to C flag. May be NULL if the C flag should not be set.
 * @return          Shifted value.
 */
uint shifter(uint num, uint amount, uint bit_size, ShiftType type, uint *C) {
    // If amount is 0, there is nothing to do.
    if (amount == 0u) {
        if (C) {
            *C = 0;
        }

        return num;
    }

    if (C) {
        *C = get_carry_bit(num, amount, bit_size, type);
    }

    switch (type) {
        // Logical shift left.
        case lsl:
            return select_range((num << amount), bit_size- 1u, 0u);
        // Logical shift right.
        case lsr:
            return select_range(num, bit_size- 1u, 0u) >> amount;
        // Arthimetic right shift (preserves sign bit).
        case asr:
            if (select_range(num, 31, 31)) {
            return shifter((1 << amount) - 1, amount, 32, ror, NULL) |
                   shifter(num, amount, 32, lsr, NULL);
            } else {
                return shifter(num, amount, 32, lsr, NULL);
            }
        // Rotate right.
        case ror:
            return shifter(num, amount, bit_size, lsr, NULL) |
                   shifter(select_range(num, amount- 1u, 0u), bit_size-amount, bit_size, lsl, NULL);
        default:
            // Invalid operation, return 0.
            return 0u;
    }
}

uint get_carry_bit(uint num, uint amount, uint bit_size, ShiftType type) {
    if (amount == 0u) {
        return 0u;
    }

    switch (type) {
        // Logical shift left
        case lsl:
            return select_range(num, bit_size-amount, bit_size-amount);
        // Anything that involves right
        case lsr:
        case asr:
        case ror:
            return select_range(num, amount- 1, amount- 1);
        default:
            // Invalid operation, return 0.
            return 0u;
    }
}

/**
 * Gets the second operand from an instruction.
 *
 * @param  state Current machine state
 * @param  C     Pointer to C flag.
 * @return       The second operand for this DP instruction.
 */
uint get_b(State *state, uint *C) {
    DPInst *instr = &(state->decoded.inst.dp);
    uint operand2 = instr->operand2;

    // Rotates the bits 7- 0 of the opcode by bits 11- 8 times 2.
    if (instr->bits_ipuasl & BIT_I) {
        return shifter(select_range(operand2, 7u, 0u),
                       select_range(operand2, 11u, 8u) * 2u, 32u, ror, C);

    // Shifts the value in a register 3- 0.
    } else {

        if (select_range(operand2, 3u, 0u) == PC) {
            // Throw tried to access PC error.
            return 0u;
        }

        uint shift_type = select_range(operand2, 6u, 5u);
        uint rm = state->registers[select_range(operand2, 3u, 0u)];

        // By the value of int 11- 7 when bit 4 = 0
        if (!select_range(operand2, 4u, 4u)) {

            return shifter(rm, select_range(operand2, 11u, 7u), 32u, shift_type, C);

        // By the value of register 11- 8 when bit 4 = 1 and bit 7 = 0.
        } else if (!select_range(operand2, 7u, 7u) && select_range(operand2, 4u, 4u)) {

            if (select_range(operand2, 11u, 8u) == PC) {
                // Throw tried to access PC error.
                return 0u;
            }

            uint rs = select_range(state->registers[select_range(operand2, 11u, 8u)], 7u, 0u);
            return shifter(rm, rs, 32u, shift_type, C);

        } else {
            // Throw unsupported instruction error
            return 0u;
        }
    }
}

/**
 * Did an addition return an overflow?
 *
 * @param  a LHS
 * @param  b RHS
 * @return   Boolean flag (0 or 1).
 */
uint is_add_overflow(uint a, uint b) {
    return (b == 0u) || (a > ~0u - b);
}

/**
 * Negate a number, while keeping it unsigned.
 *
 * @param  a Unsigned number
 * @return   2s complement negative for the number.
 */
uint to_neg(uint a) {
    return ~(a) + 1u;
}

StatusCode dp_execute(State *state) {
    DPInst *instr = &(state->decoded.inst.dp);
    uint C = 0u, N = 0u, Z = 0u;

    // Sets A to value of first operand register (Rn).
    uint a = state->registers[instr->Rn];
    // B can be either value of a specified register or an immediate value.
    uint b = get_b(state, &C);

    // Negatives of a and b
    uint neg_a, neg_b;

    uint *to_dest = state->registers + instr->Rd;
    uint result;
    switch (instr->opcode) {
        case dp_and:
        case dp_tst:
            result = a & b;
            break;
        case dp_eor:
        case dp_teq:
            result = a ^ b;
            break;
        case dp_sub:
        case dp_cmp:
            neg_b = to_neg(b);
            result = a + neg_b;
            C = is_add_overflow(a, neg_b);
            break;
        case dp_rsb:
            neg_a = to_neg(a);
            result = neg_a + b;
            C = is_add_overflow(neg_a, b);
            break;
        case dp_add:
            result = a + b;
            C = is_add_overflow(a, b);
            break;
        case dp_orr:
            result = a | b;
            break;
        case dp_mov:
            result = b;
            break;
        default:
            // Unsupported operation error
            return INVALID_OPCODE;
    }

    // Write to destination if not a non-writing operation.
    switch (instr->opcode){
        case dp_tst:
        case dp_teq:
        case dp_cmp:
            break;
        default:
            *to_dest = result;
            break;
    }

    // Set N flag if the result is negative.
    N = select_range(result, 31u, 31u);

    // Set Z flag if result is 0.
    if (result == 0u) {
        Z = 1u;
    }

    // Updates CPSR with the new flag bit
    if (instr->bits_ipuasl & BIT_S) {
        state->CPSR = select_range(state->CPSR, 28u, 0u) |
                             shifter(N << 2u | Z << 1u | C, 3u, 32u, ror, NULL);
    }

    return CONTINUE;
}

