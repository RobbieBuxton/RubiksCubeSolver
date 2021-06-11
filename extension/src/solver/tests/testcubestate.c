#include "../../../testsuite/testsuite.h"
#include "../cubestate.h"

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

static const Test TESTS[2] = {
    { .test = test_hash_cubestate, .name = "Hash cube state does not error during calculation" },
    { .test = test_solved_check, .name = "Solved function detects correctly"}
};

int main(void) {
    fprintf(stderr, "--- %s ---\n", __FILE__);
    run_tests(TESTS, sizeof(TESTS) / sizeof(Test));
    return 0;
}

