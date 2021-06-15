#include "../../../testsuite/testsuite.h"
#include "../cubestate.h"

#include <stdio.h>
#include <string.h>

static void test_movement_packing(void) {
    assert_uint_equals(1u, sizeof(Movement));
}

static void test_hash_cubestate(void) {
    CubeState state = {
        .data = { { { RED } } }
    };

    assert_uint_not_equals(0ul, hash_cubestate(&state));
}

static void test_solved_check(void) {
    assert_true(solved(&EXAMPLE_SOLVED_STATE));
    assert_false(solved(&EXAMPLE_UNSOLVED_STATE));
}

static void test_movements(void) {
    CubeState old, state;
    Movement move;

    memcpy(&old, &EXAMPLE_UNSOLVED_STATE, sizeof(CubeState));
    move.face = TOP;
    move.direction = CCW;
    state = apply_movement(&old, move);

    printCubeState(&state);
}

static const Test TESTS[4] = {
    { .test = test_movement_packing, .name = "Movement is successfully packed into one byte" },
    { .test = test_hash_cubestate, .name = "Hash cube state does not error during calculation" },
    { .test = test_solved_check, .name = "Solved function detects correctly"},
    { .test = test_movements, .name = "Apply movement works correctly"}
};

int main(void) {
    fprintf(stderr, "--- %s ---\n", __FILE__);
    run_tests(TESTS, sizeof(TESTS) / sizeof(Test));
    return 0;
}

