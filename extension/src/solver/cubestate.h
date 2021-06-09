#ifndef __CUBESTATE_H__
#define __CUBESTATE_H__

#include <stdint.h>

/**
 * Colours present on cube.
 */
typedef enum {
    RED, BLUE, YELLOW, GREEN, WHITE, ORANGE
} Colour;

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

#define FACES  6
#define HEIGHT 3
#define WIDTH  3

#define MAXIMUM_MOVEMENTS 20

// Not making the same mistake here.
typedef Colour FaceData[FACES][HEIGHT][WIDTH];

typedef enum {
    CW, CCW, DOUBLE
} Rotation;

typedef struct {
    Face face;
    Rotation direction;
} Movement;

typedef struct {
    FaceData data;
    Movement history[MAXIMUM_MOVEMENTS];
} CubeState;

// TODO
void apply_movement(CubeState *state, Movement movement);

uint64_t hash_cubestate(CubeState *state);

#endif  // __CUBESTATE_H__

