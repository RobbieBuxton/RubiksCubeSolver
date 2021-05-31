#include "helpers.h"
#include "insttypes.h"

#include <stdio.h>
#include <string.h>

uint select_bits(uint value, uint bitmask, uint offset, bool rshift_back) {
    uint masked = value & (bitmask << offset);
    return rshift_back ? (masked >> offset) : masked;
}

uint swap_endianness(uint value) {
    uint byte00to07, byte08to15, byte16to23, byte24to31;
    byte24to31 = select_bits(value, 255u, 24u, true);
    byte16to23 = select_bits(value, 255u, 16u, true);
    byte08to15 = select_bits(value, 255u,  8u, true);
    byte00to07 = select_bits(value, 255u,  0u, true);

    return (byte00to07 << 24u) | (byte08to15 << 16u) | (byte16to23 << 8u) | byte24to31;
}

sint to_twos_complement(uint unsigned_value, uint sign_bit_location) {
    sint result;

    if (sign_bit_location == 31u) {
        // Safe copy with memcpy.
        memcpy(&result, &unsigned_value, sizeof(sint));
    } else {
        // Extract sign and component.
        uint sign = select_bits(unsigned_value, 1u, sign_bit_location, false);
        uint component = unsigned_value ^ sign;

        result |= (sign << 31u) | component;
    }

    return result;
}

StatusCode load_word(State *state, uint address, Register dest) {
    uint loaded;

    // Load bytes in little endian order.
    loaded =  state->memory[address    ];
    loaded += state->memory[address + 1] << 8u;
    loaded += state->memory[address + 2] << 16u;
    loaded += state->memory[address + 3] << 24u;

    // Check register validity
    if (dest < R0 || dest > PC) {
        return FAILURE;
    }

    // Put result in destination.
    state->registers[dest] = loaded;

    return CONTINUE;
}

StatusCode store_word(State *state, uint address, Register source) {
    // Retrieve data from register
    uint data = state->registers[source];

    // Check address validity
    if (address >= MAX_MEMORY_LOCATION - sizeof(uint)) {
        return FAILURE;
    }

    // Store data in little endian order
    state->memory[address    ] = select_bits(data, 255u,  0u, true);
    state->memory[address + 1] = select_bits(data, 255u,  8u, true);
    state->memory[address + 2] = select_bits(data, 255u, 16u, true);
    state->memory[address + 3] = select_bits(data, 255u, 24u, true);

    return CONTINUE;
}

bool cond_is_true(Cond cond, uint CPSRflags) {
    switch (cond) {
        case eq:
            return select_bits(CPSRflags, 1u, 30u, false);
        case ne:
            return !cond_is_true(eq, CPSRflags);
        case ge:
            {
                uint N = select_bits(CPSRflags, 1u, 31, true);
                uint V = select_bits(CPSRflags, 1u, 28, true);
                return N == V;
            }
        case lt:
            return !cond_is_true(ge, CPSRflags);
        case gt:
            return cond_is_true(ge, CPSRflags) && cond_is_true(ne, CPSRflags);
        case le:
            return !cond_is_true(gt, CPSRflags);
        case al:
            return true;
        default:
            return false;
    }
}

bool check_decoded_cond(State *state) {
    return cond_is_true(state->decoded.condition, state->CPSR);
}

void status_code_handler(StatusCode code, State *state) {
    switch (code) {
        case FAILURE:
            printf("Error: Miscellaneous error during execution.\n");
            break;
        case INVALID_INSTRUCTION:
            printf("Error: Invalid decoded instruction: 0x%x\n", state->decoded.type);
            break;
        case INVALID_PC_LOCATION:
            printf("Error: Program counter has landed in an invalid location: %u\n", state->registers[PC]);
            break;
        case INVALID_OPCODE:
            printf("Error: Data processing instruction has an invalid opcode: 0x%08x\n", state->decoded.inst.dp.opcode);
            break;
        case FILE_OPEN_ERROR:
            printf("Error: Failed to open binary file.\n");
            break;
        case FILE_READ_ERROR:
            printf("Error: Failed to read from binary file.\n");
            break;
        case ILLEGAL_MEMORY_ACCESS:
            printf("Error: Out of bounds memory access at address 0x%08x\n", state->last_access);
        default:
            break;
    }
}

char *first_non_whitespace(char *start) {
    // Whitespace characters to look for.
    static const char *whitespace = " \r\n\t";

    for (char *c = start; *c; ++c) {
        if (!strchr(whitespace, *c)) {
            return c;
        }
    }

    return NULL;
}

InstructionType type_from_string(char *key) {
    int i;
    for (i=0; i < NUM_INSTRUCTIONS; i++) {
        InstructionTypePair pair = InstructionLookupTable[i];
        if (strcmp(pair.key, key) == 0)
            return pair.type;
    }
    return -1;
}
