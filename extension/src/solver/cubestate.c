#include "cubestate.h"

// 524287 * i == (i << 19) - i; allows optimisation.
#define HASH_CONSTANT 524287ul

uint64_t hash_cubestate(CubeState *state) {
    uint64_t hash = 0ul;
    uint64_t multiplier = 1ul;

    for (size_t f = 0; f < FACES; ++f) {
        for (size_t r = 0; r < HEIGHT; ++r) {
            for (size_t c = 0; c < WIDTH; ++c) {
                hash += multiplier * state->data[f][r][c];
                multiplier *= HASH_CONSTANT;
            }
        }
    }

    return hash;
}

