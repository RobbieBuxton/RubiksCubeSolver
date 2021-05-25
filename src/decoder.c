#include "insttypes.h"
#include "helpers.h"
#include "decoder.h"

#include <string.h>

#define BYTEMASK      255u

#define TYPEMASK      ((1u << 7u) - 1u)
#define TYPEOFFSET    22u

#define NIBBLEMASK    (15u)

#define RndOFFSET     (16u)
#define RdnOFFSET     (12u)
#define RsOFFSET      (8u)

// Unused. It is used as a reminder.
#define RmOFFSET      (0u)

#define BRANCHPATTERN (5u << 3u)
#define BRANCHMASK    (15u)

#define SDTPATTERN    (1u << 4u)
#define SDTMASK       (49u)

#define DPMASK        (3u << 4u)

StatusCode decode(State *state) {
    // First, convert the instruction to BE.
    uint raw = state->fetched;
    uint translated = swap_endianness(raw);

    // Clear out decoded instruction.
    memset(&(state->decoded), 0, sizeof(__InstUnion__));

    // Extract Cond
    state->decoded.condition = translated >> 28u;

    // Detect instruction type
    uint typemasked = select_bits(translated, TYPEMASK, TYPEOFFSET, true);
    if (!raw) {
        state->decoded.type = H;
    } else if (select_bits(typemasked, BRANCHMASK, 0u, false) == BRANCHPATTERN) {
        // Branch
        state->decoded.type = B;
        state->decoded.inst.b.offset = translated & ((1u << 24u) - 1u);
    } else if (typemasked == 0u) {
        // Multiply
        state->decoded.type = M;
        state->decoded.inst.m.bits_ipuasl = translated & BITS_IPUASL;

        state->decoded.inst.m.Rd = select_bits(translated, NIBBLEMASK, RndOFFSET, true);
        state->decoded.inst.m.Rn = select_bits(translated, NIBBLEMASK, RdnOFFSET, true);
        state->decoded.inst.m.Rs = select_bits(translated, NIBBLEMASK, RsOFFSET, true);
        state->decoded.inst.m.Rm = (translated & NIBBLEMASK);
    } else if (select_bits(typemasked, SDTMASK, 0u, false) == SDTPATTERN) {
        // Single Data Transfer
        state->decoded.type = SDT;
        state->decoded.inst.sdt.bits_ipuasl = translated & BITS_IPUASL;

        state->decoded.inst.sdt.Rn = select_bits(translated, NIBBLEMASK, RndOFFSET, true);
        state->decoded.inst.sdt.Rd = select_bits(translated, NIBBLEMASK, RdnOFFSET, true);

        state->decoded.inst.sdt.offset = translated & OPR2_OR_OFFSET;
    } else if (select_bits(typemasked, DPMASK, 0u, false) == 0u) {
        // Data Processing
        state->decoded.type = DP;
        state->decoded.inst.dp.bits_ipuasl = translated & BITS_IPUASL;

        state->decoded.inst.dp.Rn = select_bits(translated, NIBBLEMASK, RndOFFSET, true);
        state->decoded.inst.dp.Rd = select_bits(translated, NIBBLEMASK, RdnOFFSET, true);

        state->decoded.inst.dp.operand2 = translated & OPR2_OR_OFFSET;
    } else {
        // Undefined instruction
        return FAILURE;
    }

    state->flags |= BIT_DECODED;
    return CONTINUE;
}
