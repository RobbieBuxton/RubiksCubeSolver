#include "cubestate.h"
#include "movequeue.h"
#include "solver.h"

bool solve(CubeState *start, int *move_count, Movement* solution) {
    MovePriorityQueue *queue = new_move_priority_queue(100);
    add_to_move_priority_queue(queue, start, estimate_cost(start));
    CubeState *current;
    u_int64_t* visitedHashes;

    while(queue->count > 0) {
        // Get next state from the queue
        if (!poll_move_priority_queue(queue, current)) return false;

        if (!visit(current, visitedHashes)) {
            continue;
        }

        if (solved(current)) {
            *move_count = current->history_count;
            solution = current->history;
            return true;
        }

        expand_all_moves(current, queue, visitedHashes);
    }
    return false;
}

int heuristic(CubeState *state) {
    return 0;
}

int estimate_cost(CubeState *state) {
    return heuristic(state) + state->history_count;
}
