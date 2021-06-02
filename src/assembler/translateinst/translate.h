#ifndef __TRANSLATE_H__
#define __TRANSLATE_H__

#ifndef __SHORTEN__
// Full includes for language servers
#include "../../helpers/insttypes.h"
#include "../../helpers/helpers.h"
#include "../parser/symbols.h"
#include "../parser/parser.h"
#else
// Shortened includes
#include "insttypes.h"
#include "helpers.h"
#include "symbols.h"
#include "parser.h"
#endif

/**
 * Translate a Data Processing instruction.
 *
 * @param[in]  tokens         A line of assembly code separated into tokens.
 * @param[out] symbols        An associative array of symbols to the addresses.
 * @param[in]  current_offset The position of this instruction in the code.
 * @param[out] output         A memory address to which the translated instruction is to be written.
 * @param[in]  assemblyInfo   Information gained from first pass such as total length of assembly file.
 * @return                    Operation status code
 */
StatusCode dp_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output, AssemblyInfo *assemblyInfo);

/**
 * Translate a Multiply instruction.
 *
 * @param[in]  tokens         A line of assembly code separated into tokens.
 * @param[out] symbols        An associative array of symbols to the addresses.
 * @param[in]  current_offset The position of this instruction in the code.
 * @param[out] output         A memory address to which the translated instruction is to be written.
 * @param[in]  assemblyInfo   Information gained from first pass such as total length of assembly file.
 * @return                    Operation status code
 */
StatusCode m_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output, AssemblyInfo *assemblyInfo);

/**
 * Translate a Single Data Transfer instruction.
 *
 * @param[in]  tokens         A line of assembly code separated into tokens.
 * @param[out] symbols        An associative array of symbols to the addresses.
 * @param[in]  current_offset The position of this instruction in the code.
 * @param[out] output         A memory address to which the translated instruction is to be written.
 * @param[in]  assemblyInfo   Information gained from first pass such as total length of assembly file.
 * @return                    Operation status code
 */
StatusCode sdt_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output, AssemblyInfo *assemblyInfo);

/**
 * Translate a Branch instruction.
 *
 * @param[in]  tokens         A line of assembly code separated into tokens.
 * @param[out] symbols        An associative array of symbols to the addresses.
 * @param[in]  current_offset The position of this instruction in the code.
 * @param[out] output         A memory address to which the translated instruction is to be written.
 * @param[in]  assemblyInfo   Information gained from first pass such as total length of assembly file.
 * @return                    Operation status code.
 */
StatusCode b_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output, AssemblyInfo *assemblyInfo);

/**
 * Translate a Halt instruction.
 *
 * @param[in]  tokens         A line of assembly code separated into tokens.
 * @param[out] symbols        An associative array of symbols to the addresses.
 * @param[in]  current_offset The position of this instruction in the code.
 * @param[out] output         A memory address to which the translated instruction is to be written.
 * @param[in]  assemblyInfo   Information gained from first pass such as total length of assembly file.
 * @return                    Operation status code.
 */
StatusCode h_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output, AssemblyInfo *assemblyInfo);

/**
 * The type of a function that translates an array of tokens into an instruction.
 */
typedef StatusCode (*TranslateFunction)(char **, SymbolMap *, uint, uint *, AssemblyInfo *);

#endif  // __TRANSLATE_H__
