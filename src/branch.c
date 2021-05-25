#include "insttypes.h"
#include "executefuncs.h"

#include <string.h>

StatusCode b_execute(State *state) {
    // Assuming the decoder already took care of LE -> BE...
    // We just need to extract the lower 24 bits.

    // Assuming 2s complement for this encoding.
    uint bits = state->decoded.inst.b.offset;
    uint sign = bits & (1 << 24);

    // Extract component and flip accordingly
    uint component = bits ^ sign;

    // If negative, -1 and flip bits...
    if (sign) {
        component = ~(component - 1);
        component &= (1 << 24) - 1;
    }

    // Perform the jump.
    if (sign) {
        state->registers[PC] -= component;
    } else {
        state->registers[PC] += component;
    }

    // Perform checking on where we are.
    // Because the PC is unsigned, if < 0, it will wrap to a large positive value.
    // Maximum is MAX_MEMORY_LOCATION (should be 65535).
    if (state->registers[PC] >= MAX_MEMORY_LOCATION) {
        return FAILURE;
    } else {
        state->fetched = 0u;
        memset(&(state->decoded), 0, sizeof(Instruction));

        // Clear all flags
        state->flags = 0u;

        return CONTINUE;
    }
}

