#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "insttypes.h"

int main(int argc, char **argv) {
  statusCode code;
  State *machineState = initialiseState();
  while (!(code = execute(machineState))) {
    decode(machineState);
    fetch(machineState);
  }
  switch (code)
  {
  case HALT:
    return EXIT_SUCCESS
  case FAILURE:
    printf("Error")
  default:
    return EXIT_FAILURE;
  }
}

statusCode execute(State *machineState) {
  switch (machineState->decoded.type)
  {
  case DP:
    return DPExecute(machineState);
  case M:
    return MExecute(machineState);
  case SDT:
    return SDTExecute(machineState);
  case B:
    return BExecute(machineState);
  default:
    return FAILURE;
  }
}