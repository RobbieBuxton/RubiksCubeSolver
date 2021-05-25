// Remember to #define NDEBUG when you want to not run the asserts.
#include <assert.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "insttypes.h"

#include "decoder.h"
#include "fetcher.h"

#include "executefuncs.h"
#include "printState.h"
#include "helpers.h"

StatusCode execute(State *);
void initialise_state(State *);
void readFile(char *, State *);

// Memory contents for machine state.
static char main_memory[MAX_MEMORY_LOCATION] = { 0u };
static uint registers[REGISTER_COUNT] = { 0u };

int main(int argc, char **argv) {
    StatusCode code = 0;

    // Create a brand new machine state
    State machine_state;
    initialise_state(&machine_state);

    //Copy the contents of the file into the emulator's memory
    assert(argc > 1);
    readFile(argv[1], &machine_state);

    while (!code) {
        if (machine_state.flags & BIT_DECODED) {
            code = execute(&machine_state);
        }
        if (!code && machine_state.flags & BIT_FETCHED) {
            code = decode(&machine_state);
        }
        if (!code) {
            code = fetch(&machine_state);
        }
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

void readFile(char *fileName, State *machine_state) {
    FILE *file = fopen(fileName, "rb");
    fread(machine_state->memory, sizeof(uint), MAX_MEMORY_LOCATION, file);
}

StatusCode execute(State *machine_state) {
    // Current instruction is ignored if the condition is not met.
    if (!checkDecodedCond(machine_state)) {
        return CONTINUE;
    }

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
