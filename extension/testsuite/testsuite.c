#include "testsuite.h"

// We are using asserts, so we don't want to have NDEBUG.
#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <stdio.h>
#include <string.h>

void run_test(const Test test) {
    fprintf(stderr, "Running test \"%s\":\n\n", test.name);
    test.test();
    fprintf(stderr, "Test \"%s\" passed.\n\n", test.name);
}

void run_tests(const Test *tests, const size_t n) {
    fprintf(stderr, "Running %zu tests.\n\n", n);
    for (size_t i = 0; i < n; ++i) {
        fprintf(stderr, "[%zu / %zu] ", i + 1u, n);
        run_test(tests[i]);
    }

    fprintf(stderr, "All tests passed.\n");
}

void assert_true(const bool condition) {
    assert(condition);
}

void assert_false(const bool condition) {
    assert_true(!condition);
}

void assert_equals(const void *obj1, const void *obj2, const size_t size) {
    assert(memcmp(obj1, obj2, size) == 0);
}

void assert_not_equals(const void *obj1, const void *obj2, const size_t size) {
    assert(memcmp(obj1, obj2, size) != 0);
}

void assert_array_equals(const void *arr1, const void *arr2, const size_t n, const size_t size) {
    for (size_t i = 0; i < n; ++i) {
        assert_equals(arr1 + i, arr2 + i, size);
    }
}

void assert_array_not_equals(const void *arr1, const void *arr2, const size_t n, const size_t size) {
    for (size_t i = 0; i < n; ++i) {
        if (memcmp(arr1 + i, arr2 + i, size) != 0) {
            return;
        }
    }

    assert(false);
}

void assert_not_null(const void *ptr) {
    assert(ptr);
}

void assert_numm(const void *ptr) {
    assert(!ptr);
}

