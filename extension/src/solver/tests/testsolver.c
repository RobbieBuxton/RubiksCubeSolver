#include "../../../testsuite/testsuite.h"
#include "../cubestate.h"
#include "../movequeue.h"
#include "../solver.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void test_solver_solved_already(void) {
    int move_count = 5; // not 0 as it should have to change for the test to pass.
    Movement solution[MAXIMUM_MOVEMENTS] = {0};
    CubeState *start = (CubeState *) calloc(1, sizeof(CubeState));

    memcpy(start->data, &(EXAMPLE_SOLVED_STATE.data), sizeof(Colour) * 36);

    assert_true(solve(start, &move_count, solution));

    for (int move = 0; move < move_count; move++) {
        printf("direction: %u, face: %u\n", solution[move].direction, solution[move].face);
    }

    assert_sint_equals(0, move_count);
    return;
}

// This should be updated once apply_movement is explored"
static void test_solver_one_move(void) {
    int move_count = 0;
    Movement solution[MAXIMUM_MOVEMENTS] = {0};
    Movement expected_solution[MAXIMUM_MOVEMENTS] = { { .face = TOP, .direction = CCW } };
    CubeState *start = (CubeState *) calloc(1, sizeof(CubeState));

    memcpy(start->data, &EXAMPLE_UNSOLVED_STATE, sizeof(Colour) * 36);

    assert_true(solve(start, &move_count, solution));

    for (int move = 0; move < move_count; move++) {
        printf("direction: %u, face: %u\n", solution[move].direction, solution[move].face);
    }

    assert_sint_equals(1, move_count);
    assert_array_equals(expected_solution, solution, 1, sizeof(Movement));
}

static const Test TESTS[2] = {
    { .test = test_solver_solved_already, .name = "Solver runs without error and detects solved state." },
    { .test = test_solver_one_move, .name = "Solver updates output fields and can solve single move puzzle."}
};

int main(void) {
    fprintf(stderr, "--- %s ---\n", __FILE__);
    run_tests(TESTS, sizeof(TESTS) / sizeof(Test));

    return 0;
}

