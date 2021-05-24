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
