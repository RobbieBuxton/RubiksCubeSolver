#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "insttypes.h"

#include "decoder.h"
#include "fetcher.h"

#include "executefuncs.h"
#include "printState.h"

StatusCode execute(State *);
void initialise_state(State *);

// Memory contents for machine state.
static char main_memory[MAX_MEMORY_LOCATION] = { 0u };
static uint registers[REGISTER_COUNT] = { 0u };

int main(int argc, char **argv) {
    StatusCode code;

    // Create a brand new machine state
    State machine_state;
    initialise_state(&machine_state);

    while (!(code = execute(&machine_state))) {
        decode(&machine_state);
        fetch(&machine_state);
    }

    printState(&machine_state);

    switch (code) {
        case HALT:
            return EXIT_SUCCESS;
        case FAILURE:
            printf("Error");
        default:
            return EXIT_FAILURE;
    }
}

void initialise_state(State *machine_state) {
    // Clear machine state
    memset(machine_state, 0, sizeof(State));

    machine_state->memory = main_memory;
    machine_state->registers = registers;
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
