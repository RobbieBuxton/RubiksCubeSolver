#include "insttypes.h"
#include "fetcher.h"

StatusCode fetch(State *state) {
    // get current pc
    uint pc = state->registers[PC];

    // makes sure memory accesses will be valid
    if (pc + 3 >= MAX_MEMORY_LOCATION) {
        return FAILURE;
    }

    // receive bytes in order of memory, decode will decypher little-endianess
    uint fetched = state->memory[pc] << 24u;
    fetched += state->memory[pc + 1] << 16u;
    fetched += state->memory[pc + 2] << 8u;
    fetched += state->memory[pc + 3];

    // update fetched in machine pipeline
    state->fetched = fetched;

    // increment program counter
    state->registers[PC] = pc + INSTRUCTION_BYTE_LENGTH;
    state->flags |= BIT_FETCHED;

    return CONTINUE;
}
