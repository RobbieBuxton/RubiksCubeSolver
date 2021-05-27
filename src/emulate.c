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
StatusCode readFile(char *, State *);

#define LOOPCOND (!code || code == ILLEGAL_MEMORY_ACCESS)

// Memory contents for machine state.
static uchar main_memory[MAX_MEMORY_LOCATION] = { 0u };
static uint registers[REGISTER_COUNT] = { 0u };

int main(int argc, char **argv) {
    StatusCode code = 0;

    // Create a brand new machine state
    State machine_state;
    initialise_state(&machine_state);

    //Copy the contents of the file into the emulator's memory
    assert(argc > 1);
    if ((code = readFile(argv[1], &machine_state))) {
        status_code_handler(code, &machine_state);
        goto exit;
    }

    while (LOOPCOND) {
        if (machine_state.flags & BIT_DECODED) {
            code = execute(&machine_state);
            status_code_handler(code, &machine_state);
        }

        if (LOOPCOND && machine_state.flags & BIT_FETCHED) {
            code = decode(&machine_state);
            status_code_handler(code, &machine_state);
        }

        if (LOOPCOND) {
            code = fetch(&machine_state);
            status_code_handler(code, &machine_state);
        }
    }

    printState(&machine_state);

    // Handle status codes at the end of execution.
exit:
    return EXIT_SUCCESS;
}

void initialise_state(State *machine_state) {
    // Clear machine state
    memset(machine_state, 0, sizeof(State));

    machine_state->memory = main_memory;
    machine_state->registers = registers;
}

StatusCode readFile(char *fileName, State *machine_state) {
    FILE *file = fopen(fileName, "rb");

    if (!file) {
        return FILE_OPEN_ERROR;
    }

    size_t read = fread(machine_state->memory, sizeof(uchar), MAX_MEMORY_LOCATION, file);
    if (!read) {
        return FILE_READ_ERROR;
    }

    fclose(file);

    return CONTINUE;
}

StatusCode execute(State *machine_state) {
    if (machine_state->decoded.type == H) {
        return HALT;
    }

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
