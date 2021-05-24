#include "insttypes.h"
#include <string.h>

void printState(State *state) {
    uint i;
    uint contents;

    //Print the general-use registers (excluding R13 and R14)
    printf("Registers:\n");
    for (i = 0; i < 13; i++)
    {
        contents = state->registers[i];
        printf("$%-3d:%11d (0x%08x)\n", i, contents, contents);
    }

    //Print the PC and CPSR registers
    contents = state->registers[PC];
    printf("PC  :%11d (0x%08x)\n", contents, contents);
    contents = state->CPSR;
    printf("CPSR:%11d (0x%08x)\n", contents, contents);
    
    //Print the memory
    printf("Non-zero memory:\n");
    for (i = 0; i < MAX_MEMORY_LOCATION; i++)
    {
        contents = state->memory[i];
        if (contents) {
            printf("0x%08x: 0x%08x\n", i, contents);
        }
    }
}