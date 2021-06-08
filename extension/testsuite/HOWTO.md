# Test Suite Instructions

This is a simple test suite for running unit tests on code.

## Quick Start

* Include `testsuite.h` in the test source code that is calling this test suite.
* Create some test functions that match the signature `void (*TestFunction)(void)`.
* Create an array of `static constant` `Test` structs at the global level in your test source.
* Call `run_tests` using that array of `Test` structs in `main`.
* When building the test sources, link `libtestsuite.a` from this folder.
* Simply run your test code in order to run the tests.

### Example

`example.h`:
```c
#ifndef __EXAMPLE_H__
#define __EXAMPLE_H__

float fma(float a, float b, float c);

#endif
```

`example.c`:
```c
#include "example.h"

float fma(float a, float b, float c) {
    return a * b + c;
}
```

`testexample.c`:
```c
#include "example.h"
#include "testsuite.h"

void test_fma_correct_result() {
    assert_float_equals(fma(1.0f, 1.0f, 0.0f), 1.0f, 0.001f);
    assert_float_equals(fma(2.0f, 3.0f, 4.0f), 10.0f, 0.001f);
    assert_float_equals(fma(8.0f, 1.5f, 2.5f), 14.5f, 0.001f);
}

void test_fma_negatives() {
    assert_float_equals(fma(-1.0f, 1.0f, 0.0f), -1.0f, 0.001f);
    assert_float_equals(fma(1.0f, -1.0f, 0.0f), -1.0f, 0.001f);
    assert_float_equals(fma(-1.0f, -1.0f, 0.0f), 1.0f, 0.001f);
    assert_float_equals(fma(-1.0f, -1.0f, -1.0f), 0.0f, 0.001f);
    assert_float_equals(fma(-5.0f, -5.0f, 10.0f), -15.0f, 0.001f);
}

static const Test TESTS[] {
    { .test = test_fma_correct_result, .name = "Test if fma returns correct results" },
    { .test = test_fma_negatives, .name = "Test if fma correctly handles negatives" }
};

int main(void) {
    run_tests(TESTS, 2);
    return 0;
}
```

For more information, please look at `testsuite.h`.

