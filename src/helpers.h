#ifndef __HELPERS_H__
#define __HELPERS_H__

#include "insttypes.h"

/**
 * Selects a set of bits, offset by "offset" bytes.
 *
 * E.g. select_bytes(0b11001100, 0b0101, 2)
 *    = 0b11001100
 *    & 0b00010100 (0b0101 << 2 = 0b010100)
 *    ------------
 *      0b00000100
 *
 * Output for example: rshift_back ? 0b001 : 0b100
 *
 * @param  value       Value to mask
 * @param  offset      Bits to lshift mask by
 * @param  rshift_back Rshift the results back?
 * @return             See "Output for example" above.
 */
uint select_bits(uint value, uint bitmask, uint offset, char rshift_back);

/**
 * Swap the endianness of a number.
 *
 * @param  value Number to swap endianness of.
 * @return       LE -> BE, BE -> LE
 */
uint swap_endianness(uint value);

#endif  // __HELPERS_H__
