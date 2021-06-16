#include "testsuite.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_STR_LEN 1024
#define SEP         "-------------------------------------------------"

static char __last_assert_caller_file[MAX_STR_LEN] = { '\0' };
static char __message[MAX_STR_LEN] = { '\0' };
static char __last_assert_caller[MAX_STR_LEN] = { '\0' };
static int __last_line_of_assert_caller = 0;

void vbprintf(FILE *stream, const char *format, ...) {
    va_list args, argcopy;

    va_start(args, format);
    va_copy(argcopy, args);

    vsnprintf(__message, MAX_STR_LEN, format, args);

#ifdef __VERBOSE__
    vfprintf(stream, format, argcopy);
#endif

    va_end(args);
    va_end(argcopy);
}


// Internal assertion function.
static jmp_buf env;
static size_t failures = 0;

static void fail_test(void) {
    ++failures;
    longjmp(env, 1);
}

#define __test_assert__(cond) if (!(cond)) {\
    fprintf(stderr, "\n[%s] Assertion Failed. %s failed in %s at line %d:\n%s",\
            __last_assert_caller_file,\
            __func__,\
            __last_assert_caller,\
            __last_line_of_assert_caller,\
            __message\
    );\
    fail_test();\
}

// Test runner utilities.
void __set_last_file(const char *filename) {
    strcpy(__last_assert_caller_file, filename);
}

void __set_last_caller(const char *caller) {
    strcpy(__last_assert_caller, caller);
}

void __set_last_line(const int line) {
    __last_line_of_assert_caller = line;
}

void run_test(const Test test) {
    fprintf(stderr, "Running test \"%s\":\n", test.name);

    clock_t time;
    time = clock();

    if (setjmp(env) == 0) {
        test.test();
        fprintf(stderr, "\nTest passed. ");
    } else {
        fprintf(stderr, "\nTest failed. ");
    }

    time = clock() - time;

    fprintf(stderr, "\"%s\" terminated in %f seconds.\n%s\n\n",
        test.name, (double) time / CLOCKS_PER_SEC, SEP
    );
}

void run_tests(const Test *tests, const size_t n) {
    fprintf(stderr, "Running %zu tests.\n\n", n);

    clock_t time;
    time = clock();

    for (size_t i = 0; i < n; ++i) {
        fprintf(stderr, "%s\n[%zu / %zu] ", SEP, i + 1u, n);
        run_test(tests[i]);
    }

    time = clock() - time;

    fprintf(stderr, "Tests completed in %f seconds with %zu / %zu passed (%zu failed).\n\n", (double) time / CLOCKS_PER_SEC, n - failures, n, failures);
}

void __assert_true(const bool condition) {
    vbprintf(stderr, "BOOL is TRUE: %d?\n", condition);
    __test_assert__(condition);
}

void __assert_false(const bool condition) {
    vbprintf(stderr, "BOOL is FALSE: %d?\n", condition);
    __test_assert__(!condition);
}

void __assert_uint_equals(uint64_t a, uint64_t b) {
    vbprintf(stderr, "UINT EQ: %lu == %lu?\n", a, b);
    __test_assert__(a == b);
}

void __assert_uint_not_equals(uint64_t a, uint64_t b) {
    vbprintf(stderr, "UINT NEQ: %lu != %lu?\n", a, b);
    __test_assert__(a != b);
}

void __assert_sint_equals(int64_t a, int64_t b) {
    vbprintf(stderr, "SINT EQ: %ld == %ld?\n", a, b);
    __test_assert__(a == b);
}

void __assert_sint_not_equals(int64_t a, int64_t b) {
    vbprintf(stderr, "SINT NEQ: %ld != %ld?\n", a, b);
    __test_assert__(a != b);
}

void __assert_float_equals(float a, float b, float epsilon) {
    vbprintf(stderr, "FLOAT EQ: %f == %f (eps = %f)?\n", a, b, epsilon);
    float d = a - b;
    __test_assert__(d > -epsilon && d < epsilon);
}

void __assert_float_not_equals(float a, float b, float epsilon) {
    vbprintf(stderr, "FLOAT NEQ: %f != %f (eps = %f)?\n", a, b, epsilon);
    float d = a - b;
    __test_assert__(d <= -epsilon || d >= epsilon);
}

void __assert_double_equals(double a, double b, double epsilon) {
    vbprintf(stderr, "DOUBLE EQ: %f == %f (eps = %f)?\n", a, b, epsilon);
    double d = a - b;
    __test_assert__(d > -epsilon && d < epsilon);
}

void __assert_double_not_equals(double a, double b, double epsilon) {
    vbprintf(stderr, "DOUBLE NEQ: %f != %f (eps = %f)?\n", a, b, epsilon);
    double d = a - b;
    __test_assert__(d <= -epsilon || d >= epsilon);
}

void __assert_string_equals(const char *str1, const char *str2) {
    vbprintf(stderr, "STRING EQ: \"%s\" == \"%s\"?\n", str1, str2);
    __test_assert__(strcmp(str1, str2) == 0);
}

void __assert_string_not_equals(const char *str1, const char *str2) {
    vbprintf(stderr, "STRING NEQ: \"%s\" != \"%s\"?\n", str1, str2);
    __test_assert__(strcmp(str1, str2) != 0);
}

void obj_hash(const void *obj, char *out_str, const size_t total_length) {
    // Wrap between 33 (!) and 126 (~).
    char c = '!';
    for (size_t i = 0; i < total_length; ++i) {
        c += ((uint8_t *) obj)[i];
        c = (c > '~') ? (((c - '!') % ('~' - '!')) + '!') : c;
        out_str[i] = c;
    }
}

static bool in_arr_eq = false;

void __assert_equals(const void *obj1, const void *obj2, const size_t size) {
    if (!in_arr_eq) {
        char *b1, *b2;
        b1 = (char *) calloc(size + 1, sizeof(char)); b2 = (char *) calloc(size + 1, sizeof(char));
        __test_assert__(b1); __test_assert__(b2);
        obj_hash(obj1, b1, size); obj_hash(obj2, b2, size);
        vbprintf(stderr, "OBJ EQ: %s == %s?\n", b1, b2);
        free(b1); free(b2);
    }

    __test_assert__(memcmp(obj1, obj2, size) == 0);
}

void __assert_not_equals(const void *obj1, const void *obj2, const size_t size) {
    char *b1, *b2;
    b1 = (char *) calloc(size + 1, sizeof(char)); b2 = (char *) calloc(size + 1, sizeof(char));
    __test_assert__(b1); __test_assert__(b2);
    obj_hash(obj1, b1, size); obj_hash(obj2, b2, size);
    vbprintf(stderr, "OBJ NEQ: %s != %s?\n", b1, b2);
    free(b1); free(b2);

    __test_assert__(memcmp(obj1, obj2, size) != 0);
}

void __assert_array_equals(const void *arr1, const void *arr2, const size_t n, const size_t size) {
    char *b1, *b2;
    b1 = (char *) calloc(n * size + 1, sizeof(char)); b2 = (char *) calloc(n * size + 1, sizeof(char));
    __test_assert__(b1); __test_assert__(b2);
    obj_hash(arr1, b1, n * size); obj_hash(arr2, b2, n * size);
    vbprintf(stderr, "ARR EQ: %s == %s?\n", b1, b2);
    free(b1); free(b2);

    in_arr_eq = true;

    for (size_t i = 0; i < n * size; ++i) {
        __assert_equals(((uint8_t *) arr1) + i, (uint8_t *) arr2 + i, size);
    }

    in_arr_eq = false;
}

void __assert_array_not_equals(const void *arr1, const void *arr2, const size_t n, const size_t size) {
    char *b1, *b2;
    b1 = (char *) calloc(n * size + 1, sizeof(char)); b2 = (char *) calloc(n * size + 1, sizeof(char));
    __test_assert__(b1); __test_assert__(b2);
    obj_hash(arr1, b1, n * size); obj_hash(arr2, b2, n * size);
    vbprintf(stderr, "ARR NEQ: %s != %s?\n", b1, b2);
    free(b1); free(b2);

    for (size_t i = 0; i < n * size; ++i) {
        if (memcmp((uint8_t *) arr1 + i, (uint8_t *) arr2 + i, size) != 0) {
            return;
        }
    }

    __test_assert__(false);
}

void __assert_not_null(const void *ptr) {
    vbprintf(stderr, "PTR not NULL: %zu != %zu?\n", (size_t) ptr, (size_t) NULL);
    __test_assert__(ptr);
}

void __assert_null(const void *ptr) {
    vbprintf(stderr, "PTR is NULL: %zu == %zu?\n", (size_t) ptr, (size_t) NULL);
    __test_assert__(!ptr);
}

#undef __assert

