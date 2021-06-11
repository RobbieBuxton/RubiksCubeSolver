#include "cubestate.h"
#include <string.h>
#include <stdio.h>

// 524287 * i == (i << 19) - i; allows optimisation.
#define HASH_CONSTANT 524287ul

uint64_t hash_cubestate(const CubeState *state) {
    uint64_t hash = 1ul;
    uint64_t multiplier = 1ul;

    for (size_t f = 0; f < FACES; ++f) {
        for (size_t r = 0; r < SIDE_LENGTH; ++r) {
            for (size_t c = 0; c < SIDE_LENGTH; ++c) {
                hash += multiplier * state->data[f][r][c];
                multiplier *= HASH_CONSTANT;
            }
        }
    }

    return hash;
}

UnfoldTemplate get_template_of(Face face) {
    switch (face) {
        case FRONT:
            return front_unfold;
        case BACK:
            return back_unfold;
        case TOP:
            return top_unfold;
        case BOTTOM:
            return bottom_unfold;
        case LEFT:
            return left_unfold;
        default:
            return right_unfold;
    }
}

Face get_face_from_template(UnfoldTemplate template, int x, int y) {
    switch (y) {
        case 0:
            return template.neighbouring_faces[0];
        case SIDE_LENGTH + 1:
            return template.neighbouring_faces[2];
        default:
            switch (x) {
                case 0:
                    return template.neighbouring_faces[3];
                case SIDE_LENGTH + 1:
                    return template.neighbouring_faces[1];
                default:
                    return template.this_face;
            }
    }
}

const Colour *get_square_pointer(const CubeState *state, UnfoldTemplate template, int x, int y) {
    return state->data[get_face_from_template(template, x, y)][template.ys[y][x]] + template.xs[y][x];
}

void unfold(Face face, CubeState *state, UnfoldedFace output) {
    UnfoldTemplate template = get_template_of(face);
    for (size_t i = 0; i < SIDE_LENGTH + 2; i++) {
        for (size_t j = 0; j < SIDE_LENGTH + 2; j++) {
            output[i][j] = (Colour *) get_square_pointer(state, template, j, i);
        }
    }
}

void rotate(UnfoldedFace uf, Rotation rotation) {
    UnfoldedFace rotated;
    for (size_t n = 0; n <= rotation; n++) {
        for (size_t i = 0; i < SIDE_LENGTH + 2; i++) {
            for (size_t j = 0; j < SIDE_LENGTH + 2; j++) {
                rotated[i][j] = uf[SIDE_LENGTH + 1 - j][i];
            }
        }
    memcpy(uf, rotated, sizeof(UnfoldedFace));
    }
}

/*
 * Copy the colours pointed to by one UnfoldedFace to another
 */
void project(UnfoldedFace source, UnfoldedFace target) {
    for (size_t i = 0; i < SIDE_LENGTH + 2; i++) {
        for (size_t j = 0; j < SIDE_LENGTH + 2; j++) {
            // The below if makes sure we're not trying to access the corners of the array
            if (!((i == 0 || i == SIDE_LENGTH + 1) && (j == 0 || j == SIDE_LENGTH + 1))) {
                *target[i][j] = *source[i][j];
            }
        }
    }
}

CubeState apply_movement(CubeState *state, Movement movement) {
    UnfoldedFace uf;
    unfold(movement.face, state, uf);
    rotate(uf, movement.direction);

    CubeState moved;
    memcpy(&moved, state, sizeof(CubeState));
    UnfoldedFace target_uf;
    unfold(movement.face, &moved, target_uf);
    project(uf, target_uf);

    // Update move history
    moved.history[moved.history_count] = movement;
    moved.history_count++;

    return moved;
}

bool solved(const CubeState *state) {
    static const int REQUIRED_COLOUR_COUNTS = SIDE_LENGTH * SIDE_LENGTH;
    int found_colours[COLOURS] = { 0 };

    for (size_t f = 0; f < FACES; ++f) {
        Colour previous_colour = COLOURS + 1;
        for (size_t r = 0; r < SIDE_LENGTH; ++r) {
            for (size_t c = 0; c < SIDE_LENGTH; ++c) {
                Colour cur = state->data[f][r][c];

                ++(found_colours[cur]);
                if (previous_colour < COLOURS && previous_colour != cur) {
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

void printCubeState(const CubeState *state) {
    for (Face face = TOP; face < 6; face++) {
        printf("{\n");
        for (int row = 0; row < 3; row++) {
            printf("{");
            for (int col = 0; col < 3; col++) {
                char colour[7] = {0};
                switch (state->data[face][row][col])
                {
                case RED:
                    strcpy(colour, "Red   ");
                    break;
                case BLUE:
                    strcpy(colour, "Blue  ");
                    break;
                case YELLOW:
                    strcpy(colour, "Yellow");
                    break;
                case GREEN:
                    strcpy(colour, "Green ");
                    break;
                case WHITE:
                    strcpy(colour, "White ");
                    break;
                case ORANGE:
                    strcpy(colour, "Orange");
                    break;
                default:
                    strcpy(colour, "N/A   ");
                    break;
                }
                printf("%s, ", colour);
            }
            printf("}\n");
        }
        printf("},\n");
    }
    printf("move_count = %ld\n most_recent_move = %u, %u\n", state->history_count, state->history[state->history_count - 1].direction, state->history[state->history_count - 1].face);
}

