#include "executefuncs.h"

StatusCode m_execute(State *state) {
    MInst params = state->decoded.inst.m;
    uint m = state->registers[params.Rm];
    uint s = state->registers[params.Rs];

    // Calculate the result
    uint d = m * s;
    if (params.bits_ipuasl & BIT_A) {
        uint n = state->registers[params.Rn];
        d += n;
    }

    // Write the result to Rd
    state->registers[params.Rd] = d;

    // Update CPSR
    if (params.bits_ipuasl & BIT_S) {
        uint N = d & (1u << 31u);
        uint Z = (d == 0u) << 30u;
        state->CPSR = N | Z | (state->CPSR & ((1u << 30u) - 1u));
    }

    return CONTINUE;
}

