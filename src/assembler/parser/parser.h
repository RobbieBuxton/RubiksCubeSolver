#ifndef __PARSER_H__
#define __PARSER_H__

// Macros for use in parser:
#define MAXIMUM_LINE_LENGTH   512

/**
 * The different mnemonics used in the assembly.
 */
typedef enum {
    asm_add,  /**< DP: add */
    asm_sub,  /**< DP: sub */
    asm_rsb,  /**< DP: reverse sub */
    asm_and,  /**< DP: logical AND */
    asm_eor,  /**< DP: logical XOR */
    asm_orr,  /**< DP: logical OR */
    asm_mov,  /**< DP: move to register */
    asm_tst,  /**< DP: test bits */
    asm_teq,  /**< DP: test bitwise equality */
    asm_cmp,  /**< DP: compare */
    asm_mul,  /**< M: multiply */
    asm_mla,  /**< M: multiply accumulate */
    asm_ldr,  /**< SDT: load register */
    asm_str,  /**< SDT: store register */
    asm_beq,  /**< B: branch if == */
    asm_bne,  /**< B: branch if != */
    asm_bge,  /**< B: branch if >= */
    asm_blt,  /**< B: branch if < */
    asm_bgt,  /**< B: branch if > */
    asm_ble,  /**< B: branch if <= */
    asm_b,    /**< B: unconditional branch */
    asm_lsl,  /**< SPECIAL: left shift */
    asm_andeq /**< SPECIAL: halt */
} Mnemonic;

#endif  // __PARSER_H__

