#include "helpers.h"
#include "insttypes.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * Lookup table defining the types of each instruction.
 */
static InstructionTypePair InstructionLookupTable[] = {
    { "add"  , DP  },
    { "sub"  , DP  },
    { "rsb"  , DP  },
    { "and"  , DP  },
    { "eor"  , DP  },
    { "orr"  , DP  },
    { "mov"  , DP  },
    { "tst"  , DP  },
    { "teq"  , DP  },
    { "cmp"  , DP  },
    { "mul"  , M   },
    { "mla"  , M   },
    { "ldr"  , SDT },
    { "str"  , SDT },
    { "beq"  , B   },
    { "bne"  , B   },
    { "bge"  , B   },
    { "blt"  , B   },
    { "bgt"  , B   },
    { "ble"  , B   },
    { "b"    , B   },
    { "lsl"  , DP  },
    { "andeq", H   }
};

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

/**
 * Modifies the GPIO pins of the current machine state.
 *
 * @param[out] Machine state to modify
 * @param[in]  Address to write to
 * @param[in]  32-bit value to write
 * @return     Status code for the operation.
 */
static StatusCode write_gpio_pins(State *state, uint addr, uint value) {
    uint *control_row;

    switch (addr) {
        case PIN_TYPES_0_9_ADDR:
            control_row = &(state->pins.pin_types_0_9);
            printf("One GPIO pin from 0 to 9 has been accessed\n");
            goto next;
        case PIN_TYPES_10_19_ADDR:
            control_row = &(state->pins.pin_types_10_19);
            printf("One GPIO pin from 10 to 19 has been accessed\n");
            goto next;
        case PIN_TYPES_20_29_ADDR:
            control_row = &(state->pins.pin_types_20_29);
            printf("One GPIO pin from 20 to 29 has been accessed\n");

            // Clear pins if 20-29 addr given and 0 written.
            if (value == 0u) {
                state->pins.pin_types_0_9   = 0u;
                state->pins.pin_types_10_19 = 0u;
                state->pins.pin_types_20_29 = 0u;
                break;
            }
next:
            *control_row = value;
            break;
        case CONTROL_ADDR:
            printf("PIN ON\n");
            state->pins.pin_states |= value;
            break;
        case CLEAR_ADDR:
            printf("PIN OFF\n");
            state->pins.pin_states ^= (state->pins.pin_states) & value;
            break;
        default:
            // Invalid physical address.
            return PIN_ERROR;
    }

    return CONTINUE;
}

StatusCode load_word(State *state, uint address, Register dest) {
    uint loaded;

    if (address < PIN_TYPES_0_9_ADDR) {
        // Load bytes in little endian order.
        loaded =  state->memory[address    ];
        loaded += state->memory[address + 1] << 8u;
        loaded += state->memory[address + 2] << 16u;
        loaded += state->memory[address + 3] << 24u;
    } else {
        // See spec.
        switch (address) {
            case PIN_TYPES_0_9_ADDR:
                printf("One GPIO pin from 0 to 9 has been accessed\n");
                break;
            case PIN_TYPES_10_19_ADDR:
                printf("One GPIO pin from 10 to 19 has been accessed\n");
                break;
            case PIN_TYPES_20_29_ADDR:
                printf("One GPIO pin from 20 to 29 has been accessed\n");
                break;
            default:
                return PIN_ERROR;
        }

        loaded = address;
    }

    // Check register validity
    if (dest < R0 || dest > PC) {
        return FAILURE;
    }

    // Put result in destination.
    state->registers[dest] = loaded;

    return CONTINUE;
}

StatusCode store_word(State *state, uint address, Register source) {
    if (address < PIN_TYPES_0_9_ADDR) {
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
    } else {
        return write_gpio_pins(state, address, state->registers[source]);
    }
}

/**
 * Check if the current state of the CPSR is adequate for an instruction to execute.
 *
 * @param  cond       Condition to compare aganst the CPSR
 * @param  cpsr_flags Current state of the CPSR
 * @return            Satisfiability of the condition
 */
static bool cond_is_true(Cond cond, uint cpsr_flags) {
    switch (cond) {
        case eq:
            return select_bits(cpsr_flags, 1u, 30u, false);
        case ne:
            return !cond_is_true(eq, cpsr_flags);
        case ge:
            {
                uint N = select_bits(cpsr_flags, 1u, 31, true);
                uint V = select_bits(cpsr_flags, 1u, 28, true);
                return N == V;
            }
        case lt:
            return !cond_is_true(ge, cpsr_flags);
        case gt:
            return cond_is_true(ge, cpsr_flags) && cond_is_true(ne, cpsr_flags);
        case le:
            return !cond_is_true(gt, cpsr_flags);
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
            break;
        case PIN_ERROR:
            printf("Error: Invalid physical address 0x%08x\n", state->last_access);
            break;
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
    for (int i = 0; i < NUM_INSTRUCTIONS; i++) {
        InstructionTypePair pair = InstructionLookupTable[i];
        if (strcmp(pair.key, key) == 0)
            return pair.type;
    }

    return -1;
}

StatusCode check_parse_error(uint *output) {
    if (errno) {
        perror("Invalid number, caused by");

        *output = 0;
        return PARSE_ERROR;
    }

    return CONTINUE;
}

ulong hash_string(const char *str) {
    ulong result = 1ul;
    ulong multiplier = 1ul;

    // Assumes the string is null-terminated.
    for (size_t i = 0; str[i]; ++i) {
        result += str[i] * multiplier;
        multiplier *= 524287ul;
    }

    return result;
}

