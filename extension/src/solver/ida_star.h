#ifndef __IDA_STAR_H__
#define __IDA_STAR_H__

#include "cubestate.h"
#include "solver.h"
#include <string.h>
#include <stdlib.h>

typedef struct {
    CubeState contents[MAXIMUM_MOVEMENTS];
    int top_index;
} StateStack;

StateStack *new_stack();

bool push(StateStack *stack, CubeState *state);

bool pop(StateStack *stack);

bool query(StateStack *stack, CubeState *dest);

bool contains(StateStack *stack, CubeState *state);

int comp_states(const void *s1, const void *s2);

void sort_by_heuristic(CubeState *arr, int nmemb);

bool successors(CubeState *state, CubeState *dest);

int search(StateStack *path, int g, int bound, CubeState *out, bool *found);

bool ida_star(CubeState *start, CubeState *dest);

bool ida_solve(CubeState *start, int *move_count, Movement *solution);

#endif