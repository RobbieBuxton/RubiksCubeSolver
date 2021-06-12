#include "../../../testsuite/testsuite.h"
#include "../cubestate.h"
#include "../movequeue.h"
#include "../solver.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void test_solver_solved_already(void) {
    // not 0 as it should have to change for the test to pass.
    int move_count = 5;
    Movement solution[MAXIMUM_MOVEMENTS] = {0};
    CubeState *start = (CubeState *) calloc(1, sizeof(CubeState));

    memcpy(start->data, &(EXAMPLE_SOLVED_STATE.data), sizeof(FaceData));

    assert_true(solve(start, &move_count, solution));

    for (int move = 0; move < move_count; move++) {
        printf("direction: %u, face: %u\n", solution[move].direction, solution[move].face);
    }

    assert_sint_equals(0, move_count);

    free(start);
}

static void test_solver_one_move(void) {
    int move_count = 0;
    Movement solution[MAXIMUM_MOVEMENTS] = { { .face = TOP, .direction = CCW } };
    Movement expected_solution[MAXIMUM_MOVEMENTS] = { { .face = TOP, .direction = CCW } };
    CubeState *start = (CubeState *) calloc(1, sizeof(CubeState));

    memcpy(start->data, &EXAMPLE_UNSOLVED_STATE, sizeof(FaceData));

    assert_true(solve(start, &move_count, solution));

    for (int move = 0; move < move_count; move++) {
        printf("direction: %u, face: %u\n", solution[move].direction, solution[move].face);
    }

    assert_sint_equals(1, move_count);
    assert_array_equals(expected_solution, solution, 1, sizeof(Movement));

    free(start);
}

static void test_solver_scrambled(void) {
    printf("Solve cube length 6\n");
    CubeState *start = (CubeState *) calloc(1, sizeof(CubeState));
    memcpy(start->data, &EXAMPLE_SOLVED_STATE, sizeof(FaceData));

    CubeState oneMove = apply_movement(start, (Movement) { .face = TOP, .direction = CW});
    CubeState twoMoves = apply_movement(&oneMove, (Movement) { .face = FRONT, .direction = DOUBLE});
    CubeState threeMoves = apply_movement(&twoMoves, (Movement) { .face = BACK, .direction = CCW});
    CubeState fourMoves = apply_movement(&threeMoves, (Movement) { .face = LEFT, .direction = CW});
    CubeState fiveMoves = apply_movement(&fourMoves, (Movement) { .face = TOP, .direction = CCW});
    CubeState sixMoves = apply_movement(&fiveMoves, (Movement) { .face = RIGHT, .direction = CCW});
    CubeState sevenMoves = apply_movement(&sixMoves, (Movement) { .face = BOTTOM, .direction = DOUBLE});

    

    int move_count = 0;
    Movement solution[MAXIMUM_MOVEMENTS] = { { .face = TOP, .direction = CCW } };

    memcpy(start->data, &sevenMoves, sizeof(FaceData));

    assert_true(solve(start, &move_count, solution));

    memcpy(start->data, &EXAMPLE_SCRAMBLED_STATE, sizeof(FaceData));

    assert_true(solve(start, &move_count, solution));

    for (int move = 0; move < move_count; move++) {
        printf("direction: %u, face: %u\n", solution[move].direction, solution[move].face);
    }

    free(start);
}

static const Test TESTS[3] = {
    { .test = test_solver_solved_already, .name = "Solver runs without error and detects solved state" },
    { .test = test_solver_one_move, .name = "Solver updates output fields and can solve single move puzzle"},
    { .test = test_solver_scrambled, .name = "Solve an arbitrarily scrambled cube"}
};

int main(void) {
    fprintf(stderr, "--- %s ---\n", __FILE__);
    run_tests(TESTS, sizeof(TESTS) / sizeof(Test));

    return 0;
}

