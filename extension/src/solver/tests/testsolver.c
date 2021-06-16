#include "../../../testsuite/testsuite.h"
#include "../cubestate.h"
#include "../movequeue.h"
#include "../solver.h"
#include "../ida_star.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static CubeState *start;

static void test_solver_solved_already(void) {
    // not 0 as it should have to change for the test to pass.
    int move_count = 5;
    Movement solution[MAXIMUM_MOVEMENTS] = {0};

    memcpy(start->data, &(EXAMPLE_SOLVED_STATE.data), sizeof(FaceData));

    assert_true(solve(start, &move_count, solution));

    for (int move = 0; move < move_count; move++) {
        printf("direction: %u, face: %u\n", solution[move].direction, solution[move].face);
    }

    assert_sint_equals(0, move_count);
}

static void test_solver_one_move(void) {
    int move_count = 0;
    Movement solution[MAXIMUM_MOVEMENTS] = { { .face = TOP, .direction = CCW } };
    Movement expected_solution[MAXIMUM_MOVEMENTS] = { { .face = TOP, .direction = CCW } };

    memcpy(start->data, &EXAMPLE_UNSOLVED_STATE, sizeof(FaceData));

    assert_true(solve(start, &move_count, solution));

    for (int move = 0; move < move_count; move++) {
        printf("direction: %u, face: %u\n", solution[move].direction, solution[move].face);
    }

    assert_sint_equals(1, move_count);
    assert_array_equals(expected_solution, solution, 1, sizeof(Movement));
}

    int move_count = 0;
    Movement solution[MAXIMUM_MOVEMENTS] = { { .face = TOP, .direction = CCW } };

static void test_solver_scrambled(void) {
    int solved_count = 0;
    srand(time(NULL));

    for (int i = 0; i < 20; i++) {
        memcpy(start->data, &EXAMPLE_SOLVED_STATE, sizeof(FaceData));

        // srand(1u);
        int length = 5;
        fprintf(stderr, "Solve cube length %d\n", length);
        CubeState temp = *start;
        CubeState nMoves;

        for (int n = 0; n < length; n++) {
            nMoves = apply_movement(&temp, (Movement) { .face = (rand() % 6), .direction = (rand() % 3)});
            memcpy(temp.data, &nMoves, sizeof(FaceData));
        }

        memcpy(start->data, &nMoves, sizeof(FaceData));

        if (ida_solve(start, &move_count, solution)) {
            solved_count++;
            fprintf(stderr, "solved\n");
        }
    }

    // memcpy(start->data, &EXAMPLE_SCRAMBLED_STATE, sizeof(FaceData));

    // assert_true(solve(start, &move_count, solution));

    for (int move = 0; move < move_count; move++) {
        fprintf(stderr, "direction: %u, face: %u\n", solution[move].direction, solution[move].face);
    }

    fprintf(stderr, "solved this many out of 5: %d\n", solved_count);
}

static void test_k_solve_scrambled(void) {
    CubeState g1_midpoint;
    int solved_count = 0;
    srand(time(NULL));

    for (int i = 0; i < 5; i++) {
        memcpy(start->data, &EXAMPLE_SOLVED_STATE, sizeof(FaceData));

        // srand(1u);
        int length = 13;
        fprintf(stderr, "Solve cube length %d\n", length);
        CubeState temp = *start;
        CubeState nMoves;

        for (int n = 0; n < length; n++) {
            nMoves = apply_movement(&temp, (Movement) { .face = (rand() % 6), .direction = (rand() % 3)});
            memcpy(temp.data, &nMoves, sizeof(FaceData));
        }

        memcpy(start->data, &nMoves, sizeof(FaceData));

        g1_midpoint = k_solve(start, &move_count, solution);
        fprintf(stderr, "solved to g1");
        move_count = 0;
        if (g1_solve(&g1_midpoint, &move_count, solution)) {
            solved_count++;
            fprintf(stderr, "solved\n");
        }
    }

    // memcpy(start->data, &EXAMPLE_SCRAMBLED_STATE, sizeof(FaceData));

    // assert_true(solve(start, &move_count, solution));

    for (int move = 0; move < move_count; move++) {
        fprintf(stderr, "direction: %u, face: %u\n", solution[move].direction, solution[move].face);
    }

    fprintf(stderr, "solved this many out of 5: %d\n", solved_count);
}

static const Test TESTS[4] = {
    { .test = test_solver_solved_already, .name = "Solver runs without error and detects solved state" },
    { .test = test_solver_one_move, .name = "Solver updates output fields and can solve single move puzzle"},
    { .test = test_solver_scrambled, .name = "Solve an arbitrarily scrambled cube"},
    { .test = test_k_solve_scrambled, .name = "Solve cubes useing kociemba method"}
};

int main(void) {
    fprintf(stderr, "--- %s ---\n", __FILE__);

    start = (CubeState *) calloc(1, sizeof(CubeState));
    assert(start);

    run_tests(TESTS, sizeof(TESTS) / sizeof(Test));

    free(start);

    return 0;
}

