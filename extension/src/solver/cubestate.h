#ifndef __CUBESTATE_H__
#define __CUBESTATE_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Colours present on cube.
 */
typedef enum {
    RED = 0,
    BLUE = 1,
    YELLOW = 2,
    GREEN = 3,
    WHITE = 4,
    ORANGE = 5
} Colour;

#define COLOURS 6

/**
 * Enums for the 6 faces of the cube.
 * Following convention, assuming the cube is laid flat with one side facing you:
 *
 * - TOP is the top face, and BOTTOM is the face face-down on the table.
 * - LEFT and RIGHT are the faces on your view's left and right respectively.
 * - FRONT is the face facing you. BACK is facing away from you.
 */
typedef enum {
    TOP = 0,
    FRONT = 1,
    LEFT = 2,
    BACK = 3,
    RIGHT = 4,
    BOTTOM = 5
} Face;

#define FACES       6
#define SIDE_LENGTH 3

#define MAXIMUM_MOVEMENTS 20

// Not making the same mistake here.
typedef Colour FaceData[FACES][SIDE_LENGTH][SIDE_LENGTH];

/**
 * Rotation direction enum.
 */
typedef enum {
    CW = 0,     /**< Clockwise relative to face. */
    DOUBLE = 1, /**< Double rotation relative to face. */
    CCW = 2     /**< Counterclockwise relative to face. */
} Rotation;

/**
 * Pair of values that describe a movement.
 */
typedef struct {
    Face face;          /**< Face to rotate. */
    Rotation direction; /**< Rotation direction. */
} Movement;

/**
 * The current state of a cube, and the set of moves it took to get here.
 */
typedef struct {
    FaceData data;                       /**< What the faces look like. */

    size_t history_count;                /**< How much of the history is filled. */
    Movement history[MAXIMUM_MOVEMENTS]; /**< The rotation history. */
} CubeState;

/**
 * Apply a movement to a cube state.
 *
 * @param[in] state     State to move from.
 * @param[in]  movement Movement to apply.
 * @return              A new struct containing the modified state with an updated move history.
 */
CubeState apply_movement(const CubeState *state, Movement movement);

/**
 * Get the hash of a cube state.
 *
 * @param  state Cube state to hash.
 * @return       A 64-bit unsigned hash for a cube state.
 */
uint64_t hash_cubestate(const CubeState *state);

/**
 * Check whether a given cube state is in solved position.
 *
 * @param  state Cube state to check.
 * @return       True if solved.
 */
bool solved(const CubeState *state);

static const CubeState EXAMPLE_SOLVED_STATE = {
    .data = {
        {
            { RED, RED, RED },
            { RED, RED, RED },
            { RED, RED, RED }
        },
        {
            { GREEN, GREEN, GREEN },
            { GREEN, GREEN, GREEN },
            { GREEN, GREEN, GREEN }
        },
        {
            { BLUE, BLUE, BLUE },
            { BLUE, BLUE, BLUE },
            { BLUE, BLUE, BLUE }
        },
        {
            { YELLOW, YELLOW, YELLOW },
            { YELLOW, YELLOW, YELLOW },
            { YELLOW, YELLOW, YELLOW }
        },
        {
            { ORANGE, ORANGE, ORANGE },
            { ORANGE, ORANGE, ORANGE },
            { ORANGE, ORANGE, ORANGE }
        },
        {
            { WHITE, WHITE, WHITE },
            { WHITE, WHITE, WHITE },
            { WHITE, WHITE, WHITE }
        }
    },
    .history_count = 0u,
    .history = { { .face = TOP, .direction = CW } }
};

// update this once I know which way up our faces are represented
static const CubeState EXAMPLE_UNSOLVED_STATE = {
    .data = {
        {
            { RED, RED, RED },
            { RED, RED, RED },
            { RED, RED, RED }
        },
        {
            { ORANGE, ORANGE, ORANGE },
            { GREEN, GREEN, GREEN },
            { GREEN, GREEN, GREEN }
        },
        {
            { GREEN, GREEN, GREEN },
            { BLUE, BLUE, BLUE },
            { BLUE, BLUE, BLUE }
        },
        {
            { BLUE, BLUE, BLUE },
            { YELLOW, YELLOW, YELLOW },
            { YELLOW, YELLOW, YELLOW }
        },
        {
            { YELLOW, YELLOW, YELLOW },
            { ORANGE, ORANGE, ORANGE },
            { ORANGE, ORANGE, ORANGE }
        },
        {
            { WHITE, WHITE, WHITE },
            { WHITE, WHITE, WHITE },
            { WHITE, WHITE, WHITE }
        }
    },
    .history_count = 0u,
    .history = { { .face = TOP, .direction = CW } }
};

/*
 * UnfoldedFace is a 5x5 array of pointers pointing to a given face of a cube and
 * the face's neighbouring edges.
 */
typedef Colour *UnfoldedFace[SIDE_LENGTH + 2][SIDE_LENGTH + 2];

/*
 * UnfoldedFaces are generated based on pre-computed "templates" which tell you
 * how each face connects to its neighbours
 */
typedef struct {
    Face this_face;
    Face neighbouring_faces[4];
    uint8_t xs[SIDE_LENGTH + 2][SIDE_LENGTH + 2];
    uint8_t ys[SIDE_LENGTH + 2][SIDE_LENGTH + 2];
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

static const UnfoldTemplate right_unfold = {
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


#endif  // __CUBESTATE_H__

