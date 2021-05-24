#ifndef __INSTTYPES_H__
#define __INSTTYPES_H__

#include <stdint.h>

// Simplifying typedef.
typedef uint32_t uint;

/**
 * Enum for encoding the various CPSR flag combinations.
 * Enum used for conditionally executing instructions. A simple XOR can be used to check if the enums match.
 */
typedef enum {
    eq = 0b0000, /**< Z flag set; equal */
    ne = 0b0001, /**< Z clear; not equal */
    ge = 0b1010, /**< N equals V; greater-than-or-equal */
    lt = 0b1011, /**< N not equal to V; less than */
    gt = 0b1100, /**< Z clear AND (N equals V); greater than */
    le = 0b1101, /**< Z set OR (N not equal to V); less than or equal */
    al = 0b1110  /**< Unconditional execution */
} Cond;

/**
 * Data processing instruction opcodes.
 * Enum for encoding the various opcodes used in data processing instructions.
 */
typedef enum {
    dp_and = 0b0000, /**< Rn AND opr2 */
    dp_eor = 0b0001, /**< Rn EOR opr2 */
    dp_sub = 0b0010, /**< Rn - opr2 */
    dp_rsb = 0b0011, /**< opr2 - Rn */
    dp_add = 0b0100, /**< Rn + opr2 */
    dp_tst = 0b1000, /**< and, but result not written */
    dp_teq = 0b1001, /**< eor, but result not written */
    dp_cmp = 0b1010, /**< sub, but result not written */
    dp_orr = 0b1100, /**< Rn OR opr2 */
    dp_mov = 0b1101  /**< opr2 (Rn is ignored) */
} DPOpCode;

/**
 * Register names and encodings.
 * Enum used for having convenient names for referring to the registers.
 *
 * R0-R12 are general use registers.
 * R13 is unused.
 * PC is the program counter.
 * CPSR is the 4-byte register used for conditional instruction execution.
 */
typedef enum {
    R0   = 0b0000,
    R1   = 0b0001,
    R2   = 0b0010,
    R3   = 0b0011,
    R4   = 0b0100,
    R5   = 0b0101,
    R6   = 0b0110,
    R7   = 0b0111,
    R8   = 0b1000,
    R9   = 0b1001,
    R10  = 0b1010,
    R11  = 0b1011,
    R12  = 0b1100,
    R13  = 0b1101,
    R14  = 0b1110,
    PC   = 0b1111,
} Register;

/**
 * Enum defining the used instruction types.
 */
typedef enum {
    DP,  /**< Data Processing */
    M,   /**< Multiply */
    SDT, /**< Single Data Transfer */
    B    /**< Branch */
} InstructionType;

// These definitions are for extracting the I P U A S L bits from an instruction
// Use as: inst_bytes & BIT_x
#define BIT_I       (1u << 25u)
#define BIT_P       (1u << 24u)
#define BIT_U       (1u << 23u)
#define BIT_A       (1u << 21u)
#define BIT_S       (1u << 20u)
#define BIT_L       (1u << 20u)
#define BITS_IPUASL (BIT_I | BIT_P | BIT_U | BIT_A | BIT_S | BIT_L)

// Extract lower 12 bytes with &.
// E.g. inst & OPR2_OR_OFFSET
#define OPR2_OR_OFFSET ((1u << 12u) - 1u)

/**
 * Data Processing instruction struct.
 *
 * I set:   operand2 is immediate.
 * I unset: operand2 is a shifted register.
 * S set:   update CSPR flags during execution.
 * S unset: do not update CSPR flags during execution.
 */
typedef struct {
    DPOpCode opcode;  /**< Opcode for instruction. */
    uint bits_ipuasl; /**< I and S flags only. */
    Register Rn;      /**< Source register. */
    Register Rd;      /**< Destination register. */
    uint operand2;    /**< Either rotated 8b constant or shifted register. */
} DPInst;

/**
 * Multiple instruction struct.
 *
 * A set:   perform multiply-and-accumulate.
 * A unset: perform multiply only.
 * S set:   update CSPR flags during execution.
 * S unset: do not update CSPR flags during execution.
 */
typedef struct {
    uint bits_ipuasl; /**< A and S flags only. */
    Register Rd;      /**< Destination register. */
    Register Rn;      /**< Operand register n. */
    Register Rs;      /**< Operand register s. */
    Register Rm;      /**< Operand register m. */
} MInst;

/**
 * Single Data Transfer instruction struct.
 *
 * I set:   operand2 is immediate.
 * I unset: operand2 is a shifted register.
 * P set:   [pre-index] offset added/subtracted to base register before transfer.
 * P unset: [post-index] offset added/subtracted to the base register after transfer.
 * U set:   offset added to base register.
 * U unset: offset subtracted from base register.
 * L set:   load word from memory.
 * L unset: store word into memory.
 */
typedef struct {
    uint bits_ipuasl; /**< I, P, U and L flags only. */
    Register Rn;      /**< Base register */
    Register Rd;      /**< Source / Destination register. */
    uint offset;      /**< Unsigned 12 bit immediate offset or shifted register. */
} SDTInst;

/**
 * Branch instruction struct.
 * Although an unsigned 32 bit integer is used to store the offset, it is to be treated like a signed 24-bit number.
 */
typedef struct {
    uint offset;    /**< Branch offset. It is a SIGNED 24 bit number. */
} BInst;

/**
 * Instruction struct.
 * Formed by composing the specific instruction type with this struct.
 * Use type to differentiate during execution.
 */
typedef struct {
    Cond condition;       /**< Condition to execute the instruction in. */
    InstructionType type; /**< Type of instruction. */
    union {
        DPInst dp;
        MInst m;
        SDTInst sdt;
        BInst b;
    } inst;               /**< Union holding the specific instruction. Use type to check what is inside. */
} Instruction;

typedef struct {
    uint *registers;
    char *memory;
    uint fetched;
    Instruction decoded;
} State;

#endif  // __INSTTYPES_H__
