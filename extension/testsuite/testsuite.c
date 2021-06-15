#include "testsuite.h"

// We are using asserts, so we don't want to have NDEBUG.
#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// Assert abort handlers.
static jmp_buf env;
static size_t failures = 0;

static void handle_sigabrt(int signum) {
    ++failures;

    signal(signum, SIG_DFL);
    longjmp(env, 1);
}

static void run_and_handle_test_aborts(const Test test) {
    if (setjmp(env) == 0) {
        signal(SIGABRT, handle_sigabrt);
        test.test();
        signal(SIGABRT, SIG_DFL);

        fprintf(stderr, "\nTest passed. ");
    } else {
        fprintf(stderr, "\nTest failed. ");
    }
}

// Test runner utilities.
void run_test(const Test test) {
    fprintf(stderr, "Running test \"%s\":\n\n", test.name);

    clock_t time;
    time = clock();

    run_and_handle_test_aborts(test);

    time = clock() - time;

    fprintf(stderr, "\"%s\" terminated in %f seconds.\n\n", test.name, (double) time / CLOCKS_PER_SEC);
}

void run_tests(const Test *tests, const size_t n) {
    fprintf(stderr, "Running %zu tests.\n\n", n);

    clock_t time;
    time = clock();

    for (size_t i = 0; i < n; ++i) {
        fprintf(stderr, "[%zu / %zu] ", i + 1u, n);
        run_test(tests[i]);
    }

    time = clock() - time;

    fprintf(stderr, "Tests completed in %f seconds with %zu / %zu passed (%zu failed).\n\n", (double) time / CLOCKS_PER_SEC, n - failures, n, failures);
}

void assert_true(const bool condition) {
    fprintf(stderr, "BOOL is TRUE: %d?\n", condition);
    assert(condition);
}

void assert_false(const bool condition) {
    fprintf(stderr, "BOOL is FALSE: %d?\n", condition);
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
    assert(d > -epsilon && d < epsilon);
}

void assert_float_not_equals(float a, float b, float epsilon) {
    fprintf(stderr, "FLOAT NEQ: %f != %f (eps = %f)?\n", a, b, epsilon);
    float d = a - b;
    assert(d <= -epsilon || d >= epsilon);
}

void assert_double_equals(double a, double b, double epsilon) {
    fprintf(stderr, "DOUBLE EQ: %f == %f (eps = %f)?\n", a, b, epsilon);
    double d = a - b;
    assert(d > -epsilon && d < epsilon);
}

void assert_double_not_equals(double a, double b, double epsilon) {
    fprintf(stderr, "DOUBLE NEQ: %f != %f (eps = %f)?\n", a, b, epsilon);
    double d = a - b;
    assert(d <= -epsilon || d >= epsilon);
}

void assert_string_equals(const char *str1, const char *str2) {
    fprintf(stderr, "STRING EQ: \"%s\" == \"%s\"?\n", str1, str2);
    assert(strcmp(str1, str2) == 0);
}

void assert_string_not_equals(const char *str1, const char *str2) {
    fprintf(stderr, "STRING NEQ: \"%s\" != \"%s\"?\n", str1, str2);
    assert(strcmp(str1, str2) != 0);
}

static void obj_hash(const void *obj, char *out_str, const size_t total_length) {
    // Wrap between 33 (!) and 126 (~).
    char c = '!';
    for (size_t i = 0; i < total_length; ++i) {
        c += ((uint8_t *) obj)[i];
        c = (c > '~') ? (((c - '!') % ('~' - '!')) + '!') : c;
        out_str[i] = c;
    }
}

static bool in_arr_eq = false;

void assert_equals(const void *obj1, const void *obj2, const size_t size) {
    if (!in_arr_eq) {
        char *b1, *b2;
        b1 = (char *) calloc(size + 1, sizeof(char)); b2 = (char *) calloc(size + 1, sizeof(char));
        assert(b1); assert(b2);
        obj_hash(obj1, b1, size); obj_hash(obj2, b2, size);
        fprintf(stderr, "OBJ EQ: %s == %s?\n", b1, b2);
        free(b1); free(b2);
    }

    assert(memcmp(obj1, obj2, size) == 0);
}

void assert_not_equals(const void *obj1, const void *obj2, const size_t size) {
    char *b1, *b2;
    b1 = (char *) calloc(size + 1, sizeof(char)); b2 = (char *) calloc(size + 1, sizeof(char));
    assert(b1); assert(b2);
    obj_hash(obj1, b1, size); obj_hash(obj2, b2, size);
    fprintf(stderr, "OBJ NEQ: %s != %s?\n", b1, b2);
    free(b1); free(b2);

    assert(memcmp(obj1, obj2, size) != 0);
}

void assert_array_equals(const void *arr1, const void *arr2, const size_t n, const size_t size) {
    char *b1, *b2;
    b1 = (char *) calloc(n * size + 1, sizeof(char)); b2 = (char *) calloc(n * size + 1, sizeof(char));
    assert(b1); assert(b2);
    obj_hash(arr1, b1, n * size); obj_hash(arr2, b2, n * size);
    fprintf(stderr, "ARR EQ: %s == %s?\n", b1, b2);
    free(b1); free(b2);

    in_arr_eq = true;

    for (size_t i = 0; i < n * size; ++i) {
        assert_equals(((uint8_t *) arr1) + i, (uint8_t *) arr2 + i, size);
    }

    in_arr_eq = false;
}

void assert_array_not_equals(const void *arr1, const void *arr2, const size_t n, const size_t size) {
    char *b1, *b2;
    b1 = (char *) calloc(n * size + 1, sizeof(char)); b2 = (char *) calloc(n * size + 1, sizeof(char));
    assert(b1); assert(b2);
    obj_hash(arr1, b1, n * size); obj_hash(arr2, b2, n * size);
    fprintf(stderr, "ARR NEQ: %s != %s?\n", b1, b2);
    free(b1); free(b2);

    for (size_t i = 0; i < n * size; ++i) {
        if (memcmp((uint8_t *) arr1 + i, (uint8_t *) arr2 + i, size) != 0) {
            return;
        }
    }

    assert(false);
}

void assert_not_null(const void *ptr) {
    fprintf(stderr, "PTR not NULL: %zu != %zu?\n", (size_t) ptr, (size_t) NULL);
    assert(ptr);
}

void assert_numm(const void *ptr) {
    fprintf(stderr, "PTR is NULL: %zu == %zu?\n", (size_t) ptr, (size_t) NULL);
    assert(!ptr);
}

#undef __assert

