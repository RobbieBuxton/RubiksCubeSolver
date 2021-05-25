#ifndef __INSTTYPES_H__
#define __INSTTYPES_H__

#include <stdint.h>

// Simplifying typedefs.
typedef uint32_t uint;
typedef int32_t sint;

/**
 * Enum for encoding the various CPSR flag combinations.
 * Enum used for conditionally executing instructions. A simple XOR can be used to check if the enums match.
 */
typedef enum {
    eq = 0,  /**< Z flag set; equal */
    ne = 1,  /**< Z clear; not equal */
    ge = 10, /**< N equals V; greater-than-or-equal */
    lt = 11, /**< N not equal to V; less than */
    gt = 12, /**< Z clear AND (N equals V); greater than */
    le = 13, /**< Z set OR (N not equal to V); less than or equal */
    al = 14  /**< Unconditional execution */
} Cond;

/**
 * Data processing instruction opcodes.
 * Enum for encoding the various opcodes used in data processing instructions.
 */
typedef enum {
    dp_and = 0,  /**< Rn AND opr2 */
    dp_eor = 1,  /**< Rn EOR opr2 */
    dp_sub = 2,  /**< Rn - opr2 */
    dp_rsb = 3,  /**< opr2 - Rn */
    dp_add = 4,  /**< Rn + opr2 */
    dp_tst = 8,  /**< and, but result not written */
    dp_teq = 9,  /**< eor, but result not written */
    dp_cmp = 10, /**< sub, but result not written */
    dp_orr = 12, /**< Rn OR opr2 */
    dp_mov = 13  /**< opr2 (Rn is ignored) */
} DPOpCode;

// Instruction byte length
#define INSTRUCTION_BYTE_LENGTH 4u

// Register count (excluding CPSR)
#define REGISTER_COUNT 16u

/**
 * Register names and encodings.
 * Enum used for having convenient names for referring to the registers.
 *
 * R0-R12 are general use registers.
 * R13-R14 are unused.
 * PC is the program counter.
 * CPSR is the 4-byte register used for conditional instruction execution.
 */
typedef enum {
    R0   = 0,
    R1   = 1,
    R2   = 2,
    R3   = 3,
    R4   = 4,
    R5   = 5,
    R6   = 6,
    R7   = 7,
    R8   = 8,
    R9   = 9,
    R10  = 10,
    R11  = 11,
    R12  = 12,
    R13  = 13,
    R14  = 14,
    PC   = 15,
} Register;

// Maximum memory location in main memory.
#define MAX_MEMORY_LOCATION 65536u

/**
 * Enum defining the used instruction types.
 */
typedef enum {
    DP,  /**< Data Processing */
    M,   /**< Multiply */
    SDT, /**< Single Data Transfer */
    B,   /**< Branch */
    H    /**< Halt */
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
 * Multiply instruction struct.
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
 * I set:   operand2 is a shifted register.
 * I unset: operand2 is immediate. !! opposite order to DP Instruction.
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
 * Instruction union.
 * Allows more compact instruction data storage.
 */
typedef union {
    DPInst dp;   /**< Data Processing instruction member. */
    MInst m;     /**< Multiply instruction member. */
    SDTInst sdt; /**< Single Data Transfer instruction member. */
    BInst b;     /**< Branch instruction member. */
} __InstUnion__;

/**
 * Instruction struct.
 * Formed by composing the specific instruction type with this struct.
 * Use type to differentiate during execution.
 */
typedef struct {
    Cond condition;       /**< Condition to execute the instruction in. */
    InstructionType type; /**< Type of instruction. */
    __InstUnion__ inst;   /**< Union holding the specific instruction. Use type to check what is inside. */
} Instruction;

// State flags
#define BIT_FETCHED 1u
#define BIT_DECODED 2u

/**
 * Emulator state struct.
 * Keeps track of the current state of the ARM emulator. Registers and memory are allocated at runtime.
 */
typedef struct {
    char flags;          /**< Fetched and Decoded flags. Instructions are only executed if both flags are set. */
    uint *registers;     /**< The sixteen accessible registers from an instruction. */
    char *memory;        /**< The 65536 memory slots of the system. */
    uint fetched;        /**< Raw fetched instruction to be executed after decoded. */
    Instruction decoded; /**< Next decoded instruction to be executed. */
    uint CPSR;           /**< Status flags register. */
} State;

/**
 * Runtime status codes for the emulator.
 */
typedef enum {
    CONTINUE = 0, /**< Carry on execution. */
    HALT = 1,     /**< Program exited cleanly. */
    FAILURE = 2   /**< Error during execution, exit program. */
} StatusCode;

#endif  // __INSTTYPES_H__
