#include "../../../testsuite/testsuite.h"
#include "../cubestate.h"

#include <stdio.h>
#include <string.h>

static void test_movement_packing(void) {
    assert_uint_equals(1u, sizeof(Movement));
}

static void test_movement_still_allows_all_enums(void) {
    Movement movement;

    movement.direction = CW;
    assert_uint_equals(CW, movement.direction);

    movement.direction = DOUBLE;
    assert_uint_equals(DOUBLE, movement.direction);

    movement.direction = CCW;
    assert_uint_equals(CCW, movement.direction);

    movement.face = TOP;
    assert_uint_equals(TOP, movement.face);

    movement.face = FRONT;
    assert_uint_equals(FRONT, movement.face);

    movement.face = BACK;
    assert_uint_equals(BACK, movement.face);

    movement.face = LEFT;
    assert_uint_equals(LEFT, movement.face);

    movement.face = RIGHT;
    assert_uint_equals(RIGHT, movement.face);

    movement.face = BOTTOM;
    assert_uint_equals(BOTTOM, movement.face);
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

static const Test TESTS[5] = {
    { .test = test_movement_packing, .name = "Movement is successfully packed into one byte" },
    { .test = test_movement_still_allows_all_enums, .name = "Movements still have the full range of enums available" },
    { .test = test_hash_cubestate, .name = "Hash cube state does not error during calculation" },
    { .test = test_solved_check, .name = "Solved function detects correctly"},
    { .test = test_movements, .name = "Apply movement works correctly"}
};

int main(void) {
    fprintf(stderr, "--- %s ---\n", __FILE__);
    run_tests(TESTS, sizeof(TESTS) / sizeof(Test));
    return 0;
}

