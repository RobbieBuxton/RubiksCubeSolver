#include "cubestate.h"
#include "movequeue.h"
#include "solver.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool solve(CubeState *start, int *move_count, Movement *solution) {
    MovePriorityQueue *queue = new_move_priority_queue(100);
    MoveQueueNode query_result;
    add_to_move_priority_queue(queue, start, estimate_cost(start));
    HashTree* visitedHashes = new_hash_tree();
    int count = 0;
    int count2 = 0;

    while(queue->count > 0) {

        // Get next state from the queue
        if (!poll_move_priority_queue(queue, &query_result)) {
            free_hash_tree(visitedHashes);
            free_move_priority_queue(queue);

            return false;
        }
        count2++;

        if (queue->count > 4000000) {
            free_hash_tree(visitedHashes);
            free_move_priority_queue(queue);
            return false;
        }

        if (query_result.state.history_count > count) {

            printf("%d count\t", ++count);
            printf("%d count\t", count2);
            printf("%ld in queue\n", queue->count);
            // printCubeState(&(query_result.state));
        }

        if (!visit(&(query_result.state), visitedHashes)) {
            fprintf(stderr, "visited before, shouldn't have been in queue!\n");
            continue;
        }

        if (solved(&(query_result.state))) {
            *move_count = query_result.state.history_count;
            memcpy(solution, query_result.state.history, sizeof(query_result.state.history));

            free_hash_tree(visitedHashes);
            free_move_priority_queue(queue);

            return true;
        }

        expand_all_moves(&(query_result.state), queue, visitedHashes);
    }

    free_hash_tree(visitedHashes);
    free_move_priority_queue(queue);

    return false;
}

// static double spot_colour_heuristic(CubeState *state) {
//     double count = 36;
//     for (int face = TOP; face < FACES; face++) {
//         for (int row = 0; row < SIDE_LENGTH; row++) {
//             for (int col = 0; col < SIDE_LENGTH; col++) {
//                 if (MATCHES_CENTRE(face, row, col, state)) {
//                     count--;
//                 }
//             }
//         }
//     }
//     return count / 12;
// }

// static double edge_piece_heuristic(CubeState *state) {
//     double count = 12;
//     if (MATCHES_CENTRE(TOP, 0, 1, state) && MATCHES_CENTRE(BACK, 0, 1, state)) count--;
//     if (MATCHES_CENTRE(TOP, 1, 0, state) && MATCHES_CENTRE(LEFT, 0, 1, state)) count--;
//     if (MATCHES_CENTRE(TOP, 1, 2, state) && MATCHES_CENTRE(RIGHT, 0, 1, state)) count--;
//     if (MATCHES_CENTRE(TOP, 2, 1, state) && MATCHES_CENTRE(FRONT, 0, 1, state)) count--;

//     if (MATCHES_CENTRE(LEFT, 1, 0, state) && MATCHES_CENTRE(BACK, 1, 2, state)) count--;
//     if (MATCHES_CENTRE(BACK, 1, 0, state) && MATCHES_CENTRE(RIGHT, 1, 2, state)) count--;
//     if (MATCHES_CENTRE(RIGHT, 1, 0, state) && MATCHES_CENTRE(FRONT, 1, 2, state)) count--;
//     if (MATCHES_CENTRE(FRONT, 1, 0, state) && MATCHES_CENTRE(LEFT, 1, 2, state)) count--;

//     if (MATCHES_CENTRE(BOTTOM, 0, 1, state) && MATCHES_CENTRE(FRONT, 2, 1, state)) count--;
//     if (MATCHES_CENTRE(BOTTOM, 1, 0, state) && MATCHES_CENTRE(LEFT, 2, 1, state)) count--;
//     if (MATCHES_CENTRE(BOTTOM, 1, 2, state) && MATCHES_CENTRE(RIGHT, 2, 1, state)) count--;
//     if (MATCHES_CENTRE(BOTTOM, 2, 1, state) && MATCHES_CENTRE(BACK, 2, 1, state)) count--;

//     return count / 4;
// }

// static double corner_piece_heuristic(CubeState *state) {
//     double count = 4;
//     if (MATCHES_CENTRE(TOP, 0, 0, state) && MATCHES_CENTRE(BACK, 0, 2, state) && MATCHES_CENTRE(LEFT, 0, 0, state)) count--;
//     if (MATCHES_CENTRE(TOP, 0, 2, state) && MATCHES_CENTRE(RIGHT, 0, 2, state) && MATCHES_CENTRE(BACK, 0, 0, state)) count--;
//     if (MATCHES_CENTRE(TOP, 2, 2, state) && MATCHES_CENTRE(FRONT, 0, 2, state) && MATCHES_CENTRE(RIGHT, 0, 0, state)) count--;
//     if (MATCHES_CENTRE(TOP, 2, 0, state) && MATCHES_CENTRE(LEFT, 0, 2, state) && MATCHES_CENTRE(FRONT, 0, 0, state)) count--;

//     if (MATCHES_CENTRE(BOTTOM, 0, 0, state) && MATCHES_CENTRE(LEFT, 2, 2, state) && MATCHES_CENTRE(FRONT, 2, 0, state)) count--;
//     if (MATCHES_CENTRE(BOTTOM, 0, 2, state) && MATCHES_CENTRE(FRONT, 2, 2, state) && MATCHES_CENTRE(RIGHT, 2, 0, state)) count--;
//     if (MATCHES_CENTRE(BOTTOM, 2, 2, state) && MATCHES_CENTRE(RIGHT, 2, 2, state) && MATCHES_CENTRE(BACK, 2, 0, state)) count--;
//     if (MATCHES_CENTRE(BOTTOM, 2, 0, state) && MATCHES_CENTRE(BACK, 2, 2, state) && MATCHES_CENTRE(LEFT, 2, 0, state)) count--;

//     return count / 4;
// }

static double misplaced_pieces_heuristic(CubeState *state) {
    int max = 0;
    int min = 8;
    for (Face face = 0; face < FACES; face++) {
        int count = 0;
        if (!MATCHES_CENTRE(face, 0, 1, state)) count++;
        if (!MATCHES_CENTRE(face, 1, 0, state)) count++;
        if (!MATCHES_CENTRE(face, 1, 2, state)) count++;
        if (!MATCHES_CENTRE(face, 2, 1, state)) count++;
        if (MISPLACED_CORNER(face, 0, 0, state)) count++;
        if (MISPLACED_CORNER(face, 0, 2, state)) count++;
        if (MISPLACED_CORNER(face, 2, 0, state)) count++;
        if (MISPLACED_CORNER(face, 2, 2, state)) count++;
        if (max < count) max = count;
        if (min > count) min = count;
    }
    return max + min;
}

double heuristic(CubeState *state) {
    double h = 0;
    if (solved(state)) {
        return 0;
    }
    // add in more future heuristics
    // h += spot_colour_heuristic(state) * 0;
    // h += edge_piece_heuristic(state) * 0;
    // h += corner_piece_heuristic(state) * 0;
    h += misplaced_pieces_heuristic(state);
    return h;
}

double estimate_cost(CubeState *state) {
    return heuristic(state) + state->history_count;
}

bool expand_all_moves(CubeState *current, MovePriorityQueue *queue, HashTree *visitedHashes) {
    if (current->history_count == MAXIMUM_MOVEMENTS) {
        return false;
    }
    CubeState next;
    Movement movement;
    for (int direction = 0; direction < 3; direction++) {
        movement.direction = direction;
        for (int face = 0; face < 6; face++) {
            movement.face = face;
            next = apply_movement(current, movement);
            if (!query_hash_tree(visitedHashes, hash_cubestate(&next))) {
                add_to_move_priority_queue(queue, &next, estimate_cost(&next));
            }
        }
    }
    return true;
}

bool visit(CubeState *current, HashTree *visitedHashes) {
    return add_to_hash_tree(visitedHashes, hash_cubestate(current));
}

