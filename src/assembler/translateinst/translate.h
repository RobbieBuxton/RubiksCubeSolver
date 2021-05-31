#ifndef __TRANSLATE_H__
#define __TRANSLATE_H__

#include "insttypes.h"
#include "symbols.h"

/**
 * Translate a Data Processing instruction.
 * 
 * @param   tokens  A line of assembly code separated into tokens.
 * @param   symbols An associative array of symbols to the addresses.
 * @param   current_offset  The position of this instruction in the code.
 * @param   output  A memory address to which the translated instruction is to be written.
 * @return       Operation status code.
 */
StatusCode dp_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output);

/**
 * Translate a Multiply instruction.
 * 
 * @param   tokens  A line of assembly code separated into tokens.
 * @param   symbols An associative array of symbols to the addresses.
 * @param   current_offset  The position of this instruction in the code.
 * @param   output  A memory address to which the translated instruction is to be written.
 * @return       Operation status code.
 */
StatusCode m_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output);

/**
 * Translate a Single Data Transfer instruction.
 *
 * @param   tokens  A line of assembly code separated into tokens.
 * @param   symbols An associative array of symbols to the addresses.
 * @param   current_offset  The position of this instruction in the code.
 * @param   output  A memory address to which the translated instruction is to be written.
 * @return       Operation status code.
 */
StatusCode sdt_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output);

/**
 * Translate a Branch instruction.
 *
 * @param   tokens  A line of assembly code separated into tokens.
 * @param   symbols An associative array of symbols to the addresses.
 * @param   current_offset  The position of this instruction in the code.
 * @param   output  A memory address to which the translated instruction is to be written.
 * @return       Operation status code.
 */
StatusCode b_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output);


#endif __TRANSLATE_H__