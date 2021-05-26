#include "insttypes.h"
#include "executefuncs.h"

typedef enum {
    lsl = 0xb00,
    lsr = 0xb01,
    asr = 0xb10,
    ror = 0xb11
} ShiftType;

//bits are 0 indexed from the right
uint select_range(uint num, uint start_bit, uint finish_bit) {
    return  ((((1 << (start_bit + 1u)) - 1u) ^
              ((1 << finish_bit) - 1u)) & num) >> finish_bit;
}

uint shifter(uint num, uint amount, uint bit_size, ShiftType type, uint *C) {
    if (C != NULL) {
        *C = get_carry_bit(num, amount, bit_size, type); 
    } 
    switch (type) {
    //Logical shift left.
    case lsl:
        return select_range((num << amount),bit_size-1u,0u);
    //Logical shift right.
    case lsr:
        return select_range(num,bit_size-1u,0u) >> amount;
    //Arthimetic right shift (preserves sign bit).
    case asr:
        uint sign_bit = (1u << (bit_size-1u)) & num;
        return sign_bit | shifter(num,amount,bit_size-1,lsr,NULL);
    //Rotate right.
    case ror:
        return shifter(num,amount,bit_size,lsr,NULL) | 
               shifter(select_range(num,amount-1u,0u), bit_size-amount, bit_size, lsl,NULL);
    default:
        //Throw some sort of error.
        break;
    }
}

uint get_carry_bit(uint num, uint amount, uint bit_size, ShiftType type) {
    if (amount == 0u) {
        return 0u;
    }
    switch (type) {
    //Logical shift left
    case lsl:
        return select_range(num,bit_size-amount,bit_size-amount);
    //anything that involves right
    case lsr:
    case asr:
    case ror:
        return select_range(num,amount-1,amount-1);
    default:
        //Throw some sort of error
    }
}

uint get_b(State *machineState,uint *C) {
    DPInst *instr = machineState->decoded.inst.dp.opcode;
    uint *operand2 = instr->operand2;

    //Rotates the bits 7-0 of the opcode by bits 11-8 times 2.
    if (instr->bits_ipuasl & BIT_I) {
        return shifter(select_range(operand2,7u,0u),
                       select_range(operand2,11u,8u),32u,ror,*C);

    //Shifts the value in a register 3-0.
    } else {
    
        if (select_range(operand2,3,0) == PC) {
            //throw tried to access PC error.
        }

        uint shift_type = select_range(operand2,6u,5u);
        uint rm = machineState->registers[select_range(operand2,3u,0u)];
        
        //by the value of int 11-7 when bit 4 = 0
        if (!select_range(operand2,4u,4u)) {

            return shifter(rm,select_range(operand2,11u,7u),32u,shift_type,*C);

        //by the value of register 11-8 when bit 4 = 1 and bit 7 = 0.
        } else if (!select_range(operand2,7u,7u) && !select_range(operand2,4u,4u)) {
  
            if (select_range(operand2,11u,8u) == PC) {
                //throw tried to access PC error.
            }

            uint rs = machineState->registers[select_range(operand2,11u,8u)];
            return shifter(rm,rs,32u,shift_type,*C);

        } else {
            //throw unsupported instruction error
        }
        
    }
}

#define INT_MAX (int) 2147483647
#define INT_MIN -(int) 2147483648

uint is_add_overflow(uint a, uint b) {
    sint x = (sint) a;
    sint y = (sint) b;
    if ((x > 0) && (y > INT_MAX - x)) {
        return 1u;
    } else {
        return 0u;
    }
    
}

StatusCode dp_execute(State *machineState) {
    DPInst *instr = machineState->decoded.inst.dp.opcode;
    uint C;
    uint N;
    uint Z = 0u;
    //Sets A to value of first operand register (Rn).
    uint a = machineState->registers[instr->Rn];
    //B can be either value of a specified register or an immediate value.
    uint b = get_b(machineState,&C);

    uint to_dest;
    sint twos_to_dest;
    switch (instr->opcode) {
        case dp_and:
            to_dest = a & b;
            break;
        case dp_eor:
            to_dest = a ^ b;
            break;
        case dp_sub:

            break;
        case dp_rsb:
            break;
        case dp_add:
            break;
        case dp_tst:
            break;
        case dp_teq:
            break;
        case dp_cmp:
            break; 
        case dp_orr:
            to_dest = a | b;
            break;
        case dp_mov:
            to_dest = b;
            break;
        default:
            //Unsupported operation error
            break;
    }
    N = select_range(to_dest,31u,31u);
    if (to_dest == 0) {
        Z = 1u;
    }
    //Updates CPSR with the new flag bits
    machineState->CPSR = select_range(machineState->CPSR,28u,0u) |
                         shifter(N << 2u | Z << 1u | C,3u,32u,ror,NULL);
}
