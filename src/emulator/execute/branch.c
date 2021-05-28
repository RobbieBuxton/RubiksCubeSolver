#include "insttypes.h"
#include "executefuncs.h"

#include <string.h>

StatusCode b_execute(State *state) {
    // Assuming the decoder already took care of LE -> BE...
    // We just need to extract the lower 24 bits.

    // Assuming 2s complement for this encoding.
    uint bits = state->decoded.inst.b.offset;
    uint sign = bits & (1u << 23u);

    // Extract component and flip accordingly
    uint component = bits;

    // If negative, sign extend...
    if (sign) {
        component |= (255u << 24u);
    }

    // Shift-Left 2 bytes to make instruction alignment easier.
    component <<= 2u;

    // Perform the jump.
    state->registers[PC] += component;

    // Perform checking on where we are.
    // Because the PC is unsigned, if < 0, it will wrap to a large positive value.
    // Maximum is MAX_MEMORY_LOCATION (should be 65535).
    if (state->registers[PC] >= MAX_MEMORY_LOCATION) {
        return INVALID_PC_LOCATION;
    } else {
        // Clear fetched and decoded from the machine state, as if "flushing" the pipeline.
        state->fetched = 0u;
        memset(&(state->decoded), 0u, sizeof(Instruction));

        // Clear all flags
        state->flags = 0u;

        return CONTINUE;
    }
}

