#include "../../../testsuite/testsuite.h"

#include <stdio.h>

static void test_solver_blank(void) {
    return;
}

static const Test TESTS[1] = {
    { .test = test_solver_blank, .name = "PLACEHOLDER TEST, PLEASE ADD SOME" }
};

int main(void) {
    fprintf(stderr, "--- %s ---\n", __FILE__);
    run_tests(TESTS, sizeof(TESTS) / sizeof(Test));

    return 0;
}

