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
 * Convert an unsigned number to a 2s complement signed number.
 *
 * Call with 31 as signed_bit_location for default conversion.
 * Call with another value there for a smaller signed conversion.
 *
 * @param  unsigned_value    Value to convert
 * @param  sign_bit_location Where the MSB of this value is
 * @return                   The signed integer conversion
 */
sint to_twos_complement(uint unsigned_value, uint sign_bit_location);

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
bool check_decoded_cond(State *state);

/**
 * Print an error message regarding a non-halt / continue status code.
 *
 * @param code  Status code to print info for
 * @param state Current machine state
 */
void status_code_handler(StatusCode code, State *state);

/**
 * Find the first character in a string that is not whitespace.
 *
 * @param  start Where to start searching from
 * @return       A char * to the first non-whitespace, or NULL if not found
 */
char *first_non_whitespace(char *start);

/**
 * Return an instruction type from a string mnemonic.
 *
 * @param  key  The mnemonic as a string
 * @return      The corresponding InstructionType
 */
InstructionType type_from_string(char *key);

/**
 * Call this after trying to parse a string to make sure it was successful.
 *
 * @param  output A pointer to the output of the calling translate function
 *               to be set to a null value
 * @return       Either CONTINUE or PARSE_ERROR.
 */
StatusCode check_parse_error(uint *output);

/**
 * Generate the hash of a string.
 *
 * @param  str Start of string to hash
 * @return     A 64-bit unsigned number representing the hash of the string
 */
ulong hash_string(const char *str);

#endif  // __HELPERS_H__

