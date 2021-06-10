#include "../../../testsuite/testsuite.h"
#include "../cubestate.h"

static void test_hash_cubestate(void) {
    CubeState state;
    state.data[LEFT][0][2] = BLUE;

    assert_uint_not_equals(0ul, hash_cubestate(&state));
}

static const Test TESTS[1] = {
    { .test = test_hash_cubestate, .name = "Hash cube state does not error during calculation" }
};

int main(void) {
    fprintf(stderr, "--- %s ---\n", __FILE__);
    run_tests(TESTS, sizeof(TESTS) / sizeof(Test));
    return 0;
}

