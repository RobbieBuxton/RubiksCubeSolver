#include "insttypes.h"
#include "fetcher.h"

StatusCode fetch(State *machineState) {
    // get current pc
    uint pc = machineState->registers[PC];
    // makes sure memory accesses will be valid
    if (pc + 3 >= MAX_MEMORY_LOCATION) {
        return FAILURE;
    }
    // receive bytes in order of memory, decode will decypher little-endianess
    uint fetched = machineState->memory[pc] << 24u;
    fetched += machineState->memory[pc + 1] << 16u;
    fetched += machineState->memory[pc + 2] << 8u;
    fetched += machineState->memory[pc + 3];
    // update fetched in machine pipeline
    machineState->fetched = fetched;
    // increment program counter
    machineState->registers[PC] = pc + 4;
    return CONTINUE;
}