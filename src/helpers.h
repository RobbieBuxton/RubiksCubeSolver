#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <stdbool.h>

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
uint select_bits(uint value, uint bitmask, uint offset, bool rshift_back);

/**
 * Swap the endianness of a number.
 *
 * @param  value Number to swap endianness of.
 * @return       LE -> BE, BE -> LE
 */
uint swap_endianness(uint value);

/**
 * Load four bytes from the machine state memory into register.
 * 
 * @param state     Machine in which to complete transfer
 * @param address   Starting address of word in memory (lowest address).
 * @param dest      Register to be loaded into.
 * @return          Code reflecting whether load was successful.
 */
StatusCode load_word(State *state, uint address, Register dest);

/**
 * Store four bytes into the machine state memory from a register.
 * 
 * @param state     Machine in which to complete transfer
 * @param address   Starting address of where to store word.
 * @param data      Word to be stored.
 * @return          Code reflecting whether load was successful.
 */
StatusCode store_word(State *state, uint address, Register source);

/**
 * Checks whether or not the condition specified by the current decoded
 * instruction is met.
 *
 * @param  state Current state of the machine.
 * @return       A boolean indicating whether the condition specified by the
 *               current decoded instruction is true.
 */
bool checkDecodedCond(State *state);


#endif  // __HELPERS_H__
