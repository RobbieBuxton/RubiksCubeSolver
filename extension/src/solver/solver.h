#ifndef __SOLVER_H__
#define __SOLVER_H__

#include "cubestate.h"
#include "hashtree.h"
#include "movequeue.h"

#define MATCHES_CENTRE(f, r, c, cube) (cube->data[f][r][c] == cube->data[f][1][1])
#define MISPLACED_CORNER(f, r, c, cube) (cube->data[f][r][c] != cube->data[f][r][1] && cube->data[f][r][c] != cube->data[f][1][c])

/**
 * Finds a solution set of moves for a cube starting in position represented by start.
 *
 * @param[in]   start       The starting position, history should be empty.
 * @param[out]  move_count  The number of moves in the solution.
 * @param[out]  solution    An array of moves which transform start to a solved cube
 * @return                  True if a solution was found.
 *
 */
bool solve(CubeState *start, int *move_count, Movement* solution);

/**
 * Calculates estimated distance from state to a solved state.
 *
 * @param state The state to calculate a heuristic for
 * @return      The heuristic value
 *
 */
double heuristic(CubeState *state);

/**
 * Calculates estimated cost of a solution through state.
 *
 * @param state The state to calculate a heuristic for
 * @return      The estimated cost.
 *
 */
double estimate_cost(CubeState *state);

/**
 * Adds all states reachable in a single move from current to queue which have not yet been visited
 *
 * @param[in]  current          The state to move from.
 * @param[out] queue            The queue to add the new states too.
 * @param[in]  visitedHashes    A searchable array of hashes that should not be re-added.
 * @return                      True if the state was successfully expanded.
 *
 */
bool expand_all_moves(CubeState *current, MovePriorityQueue *queue, HashTree *visitedHashes);

/**
 * Checks whether the hash for current state is in visitedHashes
 * and adds it if not there
 *
 * @param   current         The state to check.
 * @param   visitedHashes   A searchable array of hashes.
 * @return                  True if it has been added (it was not previously visited).
 */
bool visit(CubeState *current, HashTree *visitedHashes);

#endif
