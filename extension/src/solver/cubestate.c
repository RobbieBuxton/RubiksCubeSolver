#include "cubestate.h"
#include <string.h>

// 524287 * i == (i << 19) - i; allows optimisation.
#define HASH_CONSTANT 524287ul

uint64_t hash_cubestate(const CubeState *state) {
    uint64_t hash = 1ul;
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

UnfoldTemplate get_template_of(Face face) {
    switch (face)
    {
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
    switch (y)
    {
    case 0:
        return template.neighbouring_faces[0];
    case WIDTH + 1:
        return template.neighbouring_faces[2];
    default:
        switch (x)
        {
        case 0:
            return template.neighbouring_faces[3];
        case WIDTH + 1:
            return template.neighbouring_faces[1];
        default:
            return template.this_face;
        };
    }
}

const Colour *get_square_pointer(const CubeState *state, UnfoldTemplate template, int x, int y) {
    return state->data[get_face_from_template(template, x, y)][template.ys[y][x]] + template.xs[y][x];
}

void unfold(Face face, const CubeState *state, UnfoldedFace output) {
    UnfoldTemplate template = get_template_of(face);
    for (size_t i = 0; i < WIDTH + 2; i++)
    {
        for (size_t j = 0; j < WIDTH + 2; j++)
        {
            output[i][j] = get_square_pointer(state, template, j, i);
        }
    }
}

void rotate(UnfoldedFace uf, Rotation rotation) {
    UnfoldedFace rotated;
    for (size_t n = 0; n <= rotation; n++)
    {    
        for (size_t i = 0; i < WIDTH + 1; i++)
        {
            for (size_t j = 0; j < WIDTH + 1; j++)
            {
                rotated[i][j] = uf[WIDTH + 1 - j][i];
            }
        }
    }
    memcpy(uf, rotated, sizeof(UnfoldedFace));
}

/*
Copy the colours pointed to by one UnfoldedFace to another
*/
void project(UnfoldedFace source, UnfoldedFace target) {
    for (size_t i = 0; i < WIDTH + 2; i++)
    {
        for (size_t j = 0; i < WIDTH + 2; j++)
        {
            //The below if makes sure we're not trying to access the corners of the array
            if (!((i == 0 || i == WIDTH + 1) && (j == 0 || j == WIDTH + 1)))
            {
                *(target[i][j]) = *(source[i][j]);
            }
        }
    }
}

CubeState apply_movement(const CubeState *state, Movement movement) {
    UnfoldedFace uf;
    unfold(movement.face, state, uf);
    rotate(uf, movement.direction);
    
    CubeState moved;
    memcpy(&moved, state, sizeof(CubeState));
    UnfoldedFace target_uf;
    unfold(movement.face, &moved, target_uf);
    project(uf, target_uf);

    //Update move history
    moved.history[moved.history_count] = movement;
    moved.history_count++;

    return moved;
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

