#include "helpers.h"

uint select_bits(uint value, uint bitmask, uint offset, char rshift_back) {
    uint masked = value & (bitmask << offset);
    return rshift_back ? (masked >> offset) : masked;
}

uint swap_endianness(uint value) {
    uint byte00to07, byte08to15, byte16to23, byte24to31;
    byte24to31 = select_bits(value, 255u, 24u, 1u);
    byte16to23 = select_bits(value, 255u, 16u, 1u);
    byte08to15 = select_bits(value, 255u,  8u, 1u);
    byte00to07 = select_bits(value, 255u,  0u, 1u);

    return (byte00to07 << 24u) | (byte08to15 << 16u) | (byte16to23 << 8u) | byte24to31;
}
