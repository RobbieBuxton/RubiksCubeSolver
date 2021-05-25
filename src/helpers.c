#include "helpers.h"

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

StatusCode load_word(State *state, uint address, Register dest) {
    uint loaded;

    // Load bytes in little endian order.
    loaded = state->memory[address];
    loaded += state->memory[address + 1] << 8u;
    loaded += state->memory[address + 2] << 16u;
    loaded += state->memory[address + 3] << 24u;

    // Put result in destination.
    state->registers[dest] = loaded;
}

StatusCode store_word(State *state, uint address, Register source) {
    // Retrieve data from register
    uint data = state->registers[source];
    // Store data in little endian order
    state->memory[address] = select_bits(data, 15, 0, true);
    state->memory[address+1] = select_bits(data, 15, 8, true);
    state->memory[address+2] = select_bits(data, 15, 16, true);
    state->memory[address+3] = select_bits(data, 15, 24, true);
}
