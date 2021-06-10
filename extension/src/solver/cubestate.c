#include "cubestate.h"

// 524287 * i == (i << 19) - i; allows optimisation.
#define HASH_CONSTANT 524287ul

uint64_t hash_cubestate(const CubeState *state) {
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

bool solved(const CubeState *state) {
    static const int REQUIRED_COLOUR_COUNTS = HEIGHT * WIDTH;
    int found_colours[COLOURS] = { 0 };

    for (size_t f = 0; f < FACES; ++f) {
        Colour previous_colour = -1;
        for (size_t r = 0; r < HEIGHT; ++r) {
            for (size_t c = 0; c < WIDTH; ++c) {
                Colour cur = state->data[f][r][c];

                ++(found_colours[cur]);
                if (previous_colour >= RED && previous_colour != cur) {
                    return false;
                } else {
                    previous_colour = cur;
                }
            }
        }
    }

    for (size_t i = 0; i < COLOURS; ++i) {
        if (found_colours[i] != REQUIRED_COLOUR_COUNTS) {
            return false;
        }
    }

    return true;
}

