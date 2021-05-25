#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "insttypes.h"
#include "decoder.h"
#include "executefuncs.h"

StatusCode execute(State *);

int main(int argc, char **argv) {
    StatusCode code;

    // Create a brand new machine state
    State machine_state;
    memset(&machine_state, 0, sizeof(State));

    // Old:
    // initialise_state(&machine_state);

    // Proposed:
    char main_memory[MAX_MEMORY_LOCATION] = { 0u };
    uint registers[REGISTER_COUNT] = { 0u };

    machine_state.memory = main_memory;
    machine_state.registers = registers;

    while (!(code = execute(&machine_state))) {
        decode(&machine_state);
        fetch(&machine_state);
    }

    switch (code) {
        case HALT:
            return EXIT_SUCCESS;
        case FAILURE:
            printf("Error");
        default:
            return EXIT_FAILURE;
    }
}

StatusCode execute(State *machine_state) {
  // cond checked here no need to do it in ur indiviudal function!
    switch (machine_state->decoded.type) {
        case DP:
            return dp_execute(machine_state);
        case M:
            return m_execute(machine_state);
        case SDT:
            return sdt_execute(machine_state);
        case B:
            return b_execute(machine_state);
        default:
            return FAILURE;
        }
}
