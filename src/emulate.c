#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "insttypes.h"
#include "executefuncs.h"
#include "printState.h"

StatusCode execute(State *);

int main(int argc, char **argv) {
    StatusCode code;
    State *machineState = initialiseState();

    while (!(code = execute(machineState))) {
        decode(machineState);
        fetch(machineState);
    }

    switch (code) {
        case HALT:
            return EXIT_SUCCESS;
        case FAILURE:
            printf("Error");
        default:
            return EXIT_FAILURE;
    }

    printState(machineState);
}

StatusCode execute(State *machineState) {
  // cond checked here no need to do it in ur indiviudal function!
    switch (machineState->decoded.type) {
        case DP:
            return dp_execute(machineState);
        case M:
            return m_execute(machineState);
        case SDT:
            return sdt_execute(machineState);
        case B:
            return b_execute(machineState);
        default:
            return FAILURE;
        }
}
