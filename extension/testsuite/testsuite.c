#include "testsuite.h"

// We are using asserts, so we don't want to have NDEBUG.
#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <string.h>

// Only compile this code if we want to intercept printf.
#ifdef INTERCEPT_PRINTF

void intr_prntform(const char *format, ...) {
    va_list args_prnt, args_put;

    // We want two copies of this list.
    va_start(args_prnt, format);
    va_start(args_put, format);

    // We still want console output...
    vprintf(format, args_prnt);
    vsnprintf(__LAST_OUTPUT__, FAKE_BUFFER_SIZE, format, args_put);

    va_end(args_prnt);
    va_end(args_put);
}

const char *get_last_output(void) {
    return __LAST_OUTPUT__;
}

#endif

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
    assert(!condition);
}

void assert_uint_equals(uint64_t a, uint64_t b) {
    fprintf(stderr, "UINT EQ: %lu == %lu?\n", a, b);
    assert(a == b);
}

void assert_uint_not_equals(uint64_t a, uint64_t b) {
    fprintf(stderr, "UINT NEQ: %lu != %lu?\n", a, b);
    assert(a != b);
}

void assert_sint_equals(int64_t a, int64_t b) {
    fprintf(stderr, "SINT EQ: %ld == %ld?\n", a, b);
    assert(a == b);
}

void assert_sint_not_equals(int64_t a, int64_t b) {
    fprintf(stderr, "SINT NEQ: %ld != %ld?\n", a, b);
    assert(a != b);
}

void assert_float_equals(float a, float b, float epsilon) {
    fprintf(stderr, "FLOAT EQ: %f == %f (eps = %f)?\n", a, b, epsilon);
    float d = a - b;
    assert (d > -epsilon && d < epsilon);
}

void assert_float_not_equals(float a, float b, float epsilon) {
    fprintf(stderr, "FLOAT NEQ: %f != %f (eps = %f)?\n", a, b, epsilon);
    float d = a - b;
    assert (d <= -epsilon || d >= epsilon);
}

void assert_double_equals(double a, double b, double epsilon) {
    fprintf(stderr, "DOUBLE EQ: %f == %f (eps = %f)?\n", a, b, epsilon);
    double d = a - b;
    assert (d > -epsilon && d < epsilon);
}

void assert_double_not_equals(double a, double b, double epsilon) {
    fprintf(stderr, "DOUBLE NEQ: %f != %f (eps = %f)?\n", a, b, epsilon);
    double d = a - b;
    assert (d <= -epsilon || d >= epsilon);
}

void assert_equals(const void *obj1, const void *obj2, const size_t size) {
    assert(memcmp(obj1, obj2, size) == 0);
}

void assert_not_equals(const void *obj1, const void *obj2, const size_t size) {
    assert(memcmp(obj1, obj2, size) != 0);
}

void assert_array_equals(const void *arr1, const void *arr2, const size_t n, const size_t size) {
    for (size_t i = 0; i < n * size; ++i) {
        assert_equals(((uint8_t *) arr1) + i, (uint8_t *) arr2 + i, size);
    }
}

void assert_array_not_equals(const void *arr1, const void *arr2, const size_t n, const size_t size) {
    for (size_t i = 0; i < n * size; ++i) {
        if (memcmp((uint8_t *) arr1 + i, (uint8_t *) arr2 + i, size) != 0) {
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

