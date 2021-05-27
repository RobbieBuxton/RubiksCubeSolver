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
#include "printstate.h"
#include "helpers.h"

/**
 * Initialise a machine state with starting values.
 *
 * @param state Machine state struct to give initial values to
 */
void initialise_state(State *state);

/**
 * Run a decoded instruction from a machine state, using the memory and registers of the provided state.
 *
 * @param  state Machine state to simulate
 * @return       The new status code after running an instruction
 */
StatusCode execute(State *state);

/**
 * Read a binary program to simulate.
 *
 * @param  file_path Path to binary file
 * @param  state     Machine state to read the file into
 * @return           File read error status
 */
StatusCode readFile(char *file_path, State *state);

// The condition of when the main loop should occur.
#define LOOPCOND (!code || code == ILLEGAL_MEMORY_ACCESS)

// Memory contents for machine state.
static uchar main_memory[MAX_MEMORY_LOCATION] = { 0u };
static uint registers[REGISTER_COUNT] = { 0u };

int main(int argc, char **argv) {
    // The machine's current error status code.
    StatusCode code = CONTINUE;

    // Create a brand new machine state
    State state;
    initialise_state(&state);

    //Copy the contents of the file into the emulator's memory
    assert(argc > 1);
    if ((code = readFile(argv[1], &state))) {
        status_code_handler(code, &state);
        goto exit;
    }

    // Run the emulator.
    // The three-stage pipeline is simulated using these three if statements.
    while (LOOPCOND) {
        // Run a decoded instruction iff a decoded instruction exists.
        if (state.flags & BIT_DECODED) {
            code = execute(&state);
            status_code_handler(code, &state);
        }

        // Decode a fetched instruction iff a fetched instruction exists.
        if (LOOPCOND && state.flags & BIT_FETCHED) {
            code = decode(&state);
            status_code_handler(code, &state);
        }

        // Fetch the next instruction from memory.
        if (LOOPCOND) {
            code = fetch(&state);
            status_code_handler(code, &state);
        }
    }

    // Print the final state of the machine.
    print_state(&state);

    // Handle status codes at the end of execution.
exit:
    return EXIT_SUCCESS;
}

void initialise_state(State *state) {
    // Clear machine state
    memset(state, 0, sizeof(State));

    state->memory = main_memory;
    state->registers = registers;
}

StatusCode readFile(char *file_path, State *state) {
    FILE *file = fopen(file_path, "rb");

    // If file does not exist, it has not been opened successfully.
    if (!file) {
        return FILE_OPEN_ERROR;
    }

    size_t read = fread(state->memory, sizeof(uchar), MAX_MEMORY_LOCATION, file);
    if (!read) {
        // If nothing was read, the file was either empty or there was a read error.
        return FILE_READ_ERROR;
    }

    // Close the file after reading it.
    fclose(file);

    return CONTINUE;
}

// Function pointers to execute functions:
static const ExecuteFunction functions[5] = { dp_execute, m_execute, sdt_execute, b_execute, h_execute };

StatusCode execute(State *state) {
    // Current instruction is ignored if the condition is not met.
    // Halt instructions are always executed.
    if (check_decoded_cond(state) || state->decoded.type == H) {
        return functions[state->decoded.type](state);
    } else {
        return CONTINUE;
    }
}

