#include "insttypes.h"
#include "decoder.h"

#include <string.h>

#define BYTEMASK 0b11111111u
#define TYPEMASK (0b111111u << 22u)

#define RndMASK (0b1111u << 16u)
#define RdnMASK (0b1111u << 12u)
#define RsMASK  (0b1111u << 8u)
#define RmMASK  (0b1111u)

StatusCode decode(State *state) {
    // First, convert the instruction to BE.
    uint raw = state->fetched;
    uint translated = 0u;

    // Use bitmasking to get to where we need to get.
    for (uint i = 3u; i <= 3u; --i) {
        uint mask = BYTEMASK << (8u * i);
        uint masked = raw & mask;

        uint inv3 = (3u - i);
        translated |= (masked << (8u * inv3));
    }

    // Clear out decoded instruction.
    memset(&(state->decoded), 0, sizeof(__InstUnion__));

    // Extract Cond
    state->decoded.condition = translated >> 28u;

    // Detect instruction type
    uint typemasked = (translated & TYPEMASK) >> 24u;
    if ((typemasked & 0b111100u) == 0b101000u) {
        // Branch
        state->decoded.type = B;
        state->decoded.inst.b.offset = translated & ((1u << 24u) - 1u);
    } else if (typemasked == 0u) {
        // Multiply
        state->decoded.type = M;
        state->decoded.inst.m.bits_ipuasl = translated & BITS_IPUASL;

        state->decoded.inst.m.Rd = (translated & RndMASK) >> 16u;
        state->decoded.inst.m.Rn = (translated & RdnMASK) >> 12u;
        state->decoded.inst.m.Rs = (translated & RsMASK) >> 8u;
        state->decoded.inst.m.Rm = (translated & RmMASK);
    } else if ((typemasked & 0b110001) == 0b010000u) {
        // Single Data Transfer
        state->decoded.type = SDT;
        state->decoded.inst.sdt.bits_ipuasl = translated & BITS_IPUASL;

        state->decoded.inst.sdt.Rn = (translated & RndMASK) >> 16u;
        state->decoded.inst.sdt.Rd = (translated & RdnMASK) >> 12u;

        state->decoded.inst.sdt.offset = translated & OPR2_OR_OFFSET;
    } else if ((typemasked & 0b110000) == 0) {
        // Data Processing
        state->decoded.type = DP;
        state->decoded.inst.dp.bits_ipuasl = translated & BITS_IPUASL;

        state->decoded.inst.dp.Rn = (translated & RndMASK) >> 16u;
        state->decoded.inst.dp.Rd = (translated & RdnMASK) >> 12u;

        state->decoded.inst.dp.operand2 = translated & OPR2_OR_OFFSET;
    } else {
        // Undefined instruction
        return FAILURE;
    }

    state->flags |= BIT_DECODED;
    return CONTINUE;
}
