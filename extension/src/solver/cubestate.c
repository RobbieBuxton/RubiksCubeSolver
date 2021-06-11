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
UnfoldedFace is a 5x5 array of pointers pointing to a given face of a cube and
the face's neighbouring edges.
*/
typedef Colour *UnfoldedFace[WIDTH + 2][WIDTH + 2];

/*
UnfoldedFaces are generated based on pre-computed "templates" which tell you
how each face connects to its neighbours
*/
typedef struct {
    Face this_face;
    Face neighbouring_faces[4];
    uint8_t xs[WIDTH + 2][WIDTH + 2];
    uint8_t ys[WIDTH + 2][WIDTH + 2];
} UnfoldTemplate;

static const UnfoldTemplate top_unfold = {
    .this_face = TOP,
    .neighbouring_faces = {BACK, RIGHT, FRONT, LEFT},

    .xs = {{0,2,1,0,0},
           {0,0,1,2,2},
           {1,0,1,2,1},
           {2,0,1,2,0},
           {0,0,1,2,0}},

    .ys = {{0,0,0,0,0},
           {0,0,0,0,0},
           {0,1,1,1,0},
           {0,2,2,2,0},
           {0,0,0,0,0}},
};

static const UnfoldTemplate bottom_unfold = {
    .this_face = BOTTOM,
    .neighbouring_faces = {FRONT, RIGHT, BACK, LEFT},

    .xs = {{0,0,1,2,0},
           {2,0,1,2,0},
           {1,0,1,2,1},
           {0,0,1,2,2},
           {0,2,1,0,0}},

    .ys = {{0,2,2,2,0},
           {2,0,0,0,2},
           {2,1,1,1,2},
           {2,2,2,2,2},
           {0,2,2,2,0}},
};

static const UnfoldTemplate front_unfold = {
    .this_face = FRONT,
    .neighbouring_faces = {TOP, RIGHT, BOTTOM, LEFT},

    .xs = {{0,0,1,2,0},
           {2,0,1,2,0},
           {2,0,1,2,0},
           {2,0,1,2,0},
           {0,0,1,2,0}},

    .ys = {{0,2,2,2,0},
           {0,0,0,0,0},
           {1,1,1,1,1},
           {2,2,2,2,2},
           {0,0,0,0,0}},
};

static const UnfoldTemplate back_unfold = {
    .this_face = BACK,
    .neighbouring_faces = {TOP, LEFT, BOTTOM, RIGHT},

    .xs = {{0,2,1,0,0},
           {2,0,1,2,0},
           {2,0,1,2,0},
           {2,0,1,2,0},
           {0,2,1,0,0}},

    .ys = {{0,0,0,0,0},
           {0,0,0,0,0},
           {1,1,1,1,1},
           {2,2,2,2,2},
           {0,2,2,2,0}},
};

static const UnfoldTemplate left_unfold = {
    .this_face = LEFT,
    .neighbouring_faces = {TOP, FRONT, BOTTOM, BACK},

    .xs = {{0,0,0,0,0},
           {2,0,1,2,0},
           {2,0,1,2,0},
           {2,0,1,2,0},
           {0,0,0,0,0}},

    .ys = {{0,0,1,2,0},
           {0,0,0,0,0},
           {1,1,1,1,1},
           {2,2,2,2,2},
           {0,2,1,0,0}},
};

UnfoldTemplate right_unfold = {
    .this_face = RIGHT,
    .neighbouring_faces = {TOP, BACK, BOTTOM, FRONT},

    .xs = {{0,2,2,2,0},
           {2,0,1,2,0},
           {2,0,1,2,0},
           {2,0,1,2,0},
           {0,2,2,2,0}},

    .ys = {{0,2,1,0,0},
           {0,0,0,0,0},
           {1,1,1,1,1},
           {2,2,2,2,2},
           {0,0,1,2,0}},
};

Face get_face_from_template(UnfoldTemplate *template, int x, int y) {
    switch (y)
    {
    case 0:
        return template->neighbouring_faces[0];
    case WIDTH + 1:
        return template->neighbouring_faces[2];
    default:
        switch (x)
        {
        case 0:
            return template->neighbouring_faces[3];
        case WIDTH + 1:
            return template->neighbouring_faces[1];
        default:
            return template->this_face;
        };
    }
}

Colour *get_square_pointer(CubeState *state, UnfoldTemplate *template, int x, int y) {
    return state->data[get_face_from_template(template, x, y)][template->ys[y][x]] + template->xs[y][x];
}

UnfoldTemplate *get_template_of(Face face) {
    switch (face)
    {
    case FRONT:
        return &front_unfold;
    case BACK:
        return &back_unfold;
    case TOP:
        return &top_unfold;
    case BOTTOM:
        return &bottom_unfold;
    case LEFT:
        return &left_unfold;
    case RIGHT:
        return &right_unfold;
    default:
        return NULL;
    }

}

void unfold(Face face, CubeState *state, Colour ***output) {
    UnfoldTemplate *template = get_template_of(face);
    for (size_t i = 0; i < WIDTH + 2; i++)
    {
        for (size_t j = 0; j < WIDTH + 2; j++)
        {
            output[i][j] = get_square_pointer(state, template, j, i);
        }
    }
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

