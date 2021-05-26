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
int exit_code_handler(StatusCode, State *);

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
    if ((code = readFile(argv[1], &machine_state))) {
        goto exit_handling;
    }

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

    // Handle status codes at the end of execution.
exit_handling:
    return exit_code_handler(code, &machine_state);
}

int exit_code_handler(StatusCode code, State* state) {
    switch (code) {
        case HALT:
            return EXIT_SUCCESS;
        case FAILURE:
            printf("Miscellaneous error during execution.\n");
            break;
        case INVALID_INSTRUCTION:
            printf("Invalid decoded instruction: %X\n", state->fetched);
            break;
        case INVALID_PC_LOCATION:
            printf("Program counter has landed in an invalid location: %u\n", state->registers[PC]);
            break;
        case INVALID_OPCODE:
            printf("Data processing instruction has an invalid opcode: %X\n", state->decoded.inst.dp.opcode);
            break;
        case FILE_OPEN_ERROR:
            printf("Failed to open binary file.\n");
            break;
        case FILE_READ_ERROR:
            printf("Failed to read from binary file.\n");
        default:
            break;
    }

    return EXIT_FAILURE;
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

    size_t read = fread(machine_state->memory, sizeof(uint), MAX_MEMORY_LOCATION, file);
    if (!read) {
        return FILE_READ_ERROR;
    }

    return CONTINUE;
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
        case H:
            return HALT;
        default:
            return FAILURE;
    }
}
