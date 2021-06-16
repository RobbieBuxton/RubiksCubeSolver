#include "ida_star.h"
#include "cubestate.h"
#include "solver.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



StateStack *new_stack() {
    StateStack *stack = (StateStack *) malloc(sizeof(StateStack));
    stack->top_index = -1;
    return stack;
}

bool push(StateStack *stack, CubeState *state) {
    if (stack->top_index == MAXIMUM_MOVEMENTS - 1)
    {
        return false;
    }
    else
    {
        (stack->top_index)++;
        memcpy(&(stack->contents[stack->top_index]), state, sizeof(CubeState));
        return true;
    }
}

bool pop(StateStack *stack) {
    if (stack->top_index == -1)
    {
        return false;
    }
    else
    {
        (stack->top_index)--;
        return true;
    }
}

bool query(StateStack *stack, CubeState *dest) {
    if (stack->top_index == -1)
    {
        return false;
    }
    else
    {
        memcpy(dest, &(stack->contents[stack->top_index]), sizeof(CubeState));
        return true;
    }
}

bool contains(StateStack *stack, CubeState *state) {
    uint64_t hash = hash_cubestate(state);

    for (int i = 0; i <= stack->top_index; i++) {
        if (hash == hash_cubestate(&(stack->contents[i]))) return true;
    }
    return false;
}

int comp_states(const void *s1, const void *s2) {
    int h1 = heuristic((CubeState *)s1);
    int h2 = heuristic((CubeState *)s2);
    return (h1 > h2) - (h2 > h1);
}

void sort_by_heuristic(CubeState *arr, int nmemb) {
    qsort(arr, nmemb, sizeof(CubeState), comp_states);
}

bool successors(CubeState *state, CubeState *dest) {
    Movement move;
    for (Face f = 0; f < 6; f++)
    {
        move.face = f;
        for (URotation r = 0; r < 3; r++)
        {
            move.direction = r;
            dest[3 * f + r] = apply_movement(state, move);
        }
    }
    sort_by_heuristic(dest, 18);
    return true;
}

int search(StateStack *path, int g, int bound, CubeState *out, bool *found) {
    *found = false;
    CubeState node;
    query(path, &node);
    int f = g + heuristic(&node);
    if (f > bound) return f;
    if (solved(&node)) {
        *out = node;
        *found = true;
        return node.history_count;
    }
    int min = INT32_MAX;
    CubeState succs[18];
    successors(&node, succs);
    for (int i = 0; i < 18; i++) {
        if (!contains(path, &(succs[i]))) {
            push(path, &(succs[i]));
            int t = search(path, g + 1, bound, out, found);
            if (found) return t;
            if (t < min) min = t;
            pop(path);
        }
    }
    return min;
}

bool ida_star(CubeState *start, CubeState *dest) {
    double bound = heuristic(start);
    StateStack *path = new_stack();
    push(path, start);
    while (true) {
        bool found;
        int t = search(path, 0, bound, dest, &found);
        if (found) {free(path); return true;}
        if (t == INT32_MAX) {free(path); return false;}
        bound = t;
    }
}

bool ida_solve(CubeState *start, int *move_count, Movement *solution) {
    CubeState solved_state;
    if (ida_star(start, &solved_state)) {
    *move_count = solved_state.history_count;
    memcpy(solution, solved_state.history, sizeof(solved_state.history));
    return true;
    }
    return false;
}

