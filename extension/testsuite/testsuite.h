#ifndef __TESTSUITE_H__
#define __TESTSUITE_H__

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Test functions are essentially void functions that take no arguments.
 *
 * They usually will contain at least one assert.
 */
typedef void (*TestFunction)(void);

#define TEST_NAME_MAX_LENGTH 512

/**
 * A pair struct holding a test function and the test's name.
 * E.g. test: test_ints_equal | name: "test if two returned ints are equal"
 */
typedef struct {
    TestFunction test;               /**< Pointer to test function. */
    char name[TEST_NAME_MAX_LENGTH]; /**< Name or objective of function. */
} Test;

/**
 * Run a single test.
 *
 * @param test Test to run.
 */
void run_test(const Test test);

/**
 * Run an array of tests.
 *
 * @param tests Array of tests to run.
 * @param n     How many tests are in that array.
 */
void run_tests(const Test *tests, const size_t n);

/*
 * These assertion functions are used to test parts of the code inside a test function.
 *
 * For assertion functions that take void * parameters, it is expected that the size of the object being compared is
 * given, and that at the very least, both objects are of the same size.
 *
 * Please be warned that if a struct contains a pointer, the equality check is not a deep check and will not check the
 * value that the pointer holds. Same goes for arrays. An array of pointers will only check the pointer addresses, not
 * the values being pointed to by the pointers in the arrays.
 */

void __set_last_file(const char *filename);

void __set_last_caller(const char *caller);

void __set_last_line(const int line);

#define __gen_assert__(name, ...) {\
    __set_last_file(__FILE__);\
    __set_last_caller(__func__);\
    __set_last_line(__LINE__);\
    name(__VA_ARGS__);\
}

/**
 * Assert that a condition holds.
 *
 * @param condition The condition to check.
 */
void __assert_true(const bool condition);
#define assert_true(condition) __gen_assert__(__assert_true, condition)

/**
 * Assert that a condition does not hold.
 *
 * @param condition The condition to check.
 */
void __assert_false(const bool condition);
#define assert_false(condition) __gen_assert__(__assert_false, condition)

/**
 * Assert that two unsigned integers are equal.
 * Smaller unsigned integers should be casted (sign-extended) to 64 bits.
 *
 * @param a First unsigned integer.
 * @param b Second unsigned integer.
 */
void __assert_uint_equals(uint64_t a, uint64_t b);
#define assert_uint_equals(a, b) __gen_assert__(__assert_uint_equals, a, b)

/**
 * Assert that two unsigned integers are not equal.
 * Smaller unsigned integers should be casted (sign-extended) to 64 bits.
 *
 * @param a First unsigned integer.
 * @param b Second unsigned integer.
 */
void __assert_uint_not_equals(uint64_t a, uint64_t b);
#define assert_uint_not_equals(a, b) __gen_assert__(__assert_uint_not_equals, a, b)

/**
 * Assert that two signed integers are equal.
 * Smaller signed integers should be casted (sign-extended) to 64 bits.
 *
 * @param a First signed integer.
 * @param b Second signed integer.
 */
void __assert_sint_equals(int64_t a, int64_t b);
#define assert_sint_equals(a, b) __gen_assert__(__assert_sint_equals, a, b)

/**
 * Assert that two signed integers are not equal.
 * Smaller signed integers should be casted (sign-extended) to 64 bits.
 *
 * @param a First signed integer.
 * @param b Second signed integer.
 */
void __assert_sint_not_equals(int64_t a, int64_t b);
#define assert_sint_not_equals(a, b) __gen_assert__(__assert_sint_not_equals, a, b)

/**
 * Assert that two floats are "close enough to be equal".
 * Where abs(a - b) < epsilon.
 *
 * @param a       First single-precision floating point number.
 * @param b       Second single-precision floating point number.
 * @param epsilon Maximum difference (exclusive) between the two numbers.
 */
void __assert_float_equals(float a, float b, float epsilon);
#define assert_float_equals(a, b, epsilon) __gen_assert__(__assert_float_equals, a, b, epsilon)

/**
 * Assert that two floats are not "close enough to be equal".
 * Where abs(a - b) >= epsilon.
 *
 * @param a       First single-precision floating point number.
 * @param b       Second single-precision floating point number.
 * @param epsilon Minimum difference (inclusive) between the two numbers.
 */
void __assert_float_not_equals(float a, float b, float epsilon);
#define assert_float_not_equals(a, b, epsilon) __gen_assert__(__assert_float_not_equals, a, b, epsilon)

/**
 * Assert that two doubles are "close enough to be equal".
 * Where abs(a - b) < epsilon.
 *
 * @param a       First double-precision floating point number.
 * @param b       Second double-precision floating point number.
 * @param epsilon Maximum difference (exclusive) between the two numbers.
 */
void __assert_double_equals(double a, double b, double epsilon);
#define assert_double_equals(a, b, epsilon) __gen_assert__(__assert_double_equals, a, b, epsilon)

/**
 * Assert that two doubles are not "close enough to be equal".
 * Where abs(a - b) >= epsilon.
 *
 * @param a       First double-precision floating point number.
 * @param b       Second double-precision floating point number.
 * @param epsilon Minimum difference (inclusive) between the two numbers.
 */
void __assert_double_not_equals(double a, double b, double epsilon);
#define assert_double_not_equals(a, b, epsilon) __gen_assert__(__assert_double_not_equals, a, b, epsilon)

/**
 * Assert that two (null-terminated) strings are equal (contain the same characters).
 *
 * @param str1 First string.
 * @param str2 String to compare with first.
 */
void __assert_string_equals(const char *str1, const char *str2);
#define assert_string_equals(str1, str2) __gen_assert__(__assert_string_equals, str1, str2)

/**
 * Assert that two (null-terminated) strings are not equal (have differences in their characters).
 *
 * @param str1 First string.
 * @param str2 String to compare with first.
 */
void __assert_string_not_equals(const char *str1, const char *str2);
#define assert_string_not_equals(str1, str2) __gen_assert__(__assert_string_not_equals, str1, str2)

/**
 * Assert that two objects are equal.
 *
 * PRE: no object pointer is null.
 * PRE: both objects are the same size.
 *
 * @param obj1 Pointer to the first object to compare.
 * @param obj2 Pointer to the second object to compare.
 * @param size Size of both objects.
 */
void __assert_equals(const void *obj1, const void *obj2, const size_t size);
#define assert_equals(obj1, obj2, size) __gen_assert__(__assert_equals, obj1, obj2, size)

/**
 * Assert that two objects are not equal.
 *
 * PRE: no object pointer is null.
 * PRE: both objects are the same size.
 *
 * @param obj1 Pointer to the first object to compare.
 * @param obj2 Pointer to the second object to compare.
 * @param size Size of both objects.
 */
void __assert_not_equals(const void *obj1, const void *obj2, const size_t size);
#define assert_not_equals(obj1, obj2, size) __gen_assert__(__assert_not_equals, obj1, obj2, size)

/**
 * Assert that two arrays are equal (that is, they contain the same items).
 *
 * PRE: no pointer is null.
 * PRE: both arrays contain objects that are the same size.
 * PRE: both arrays contain at least n items.
 *
 * @param arr1 Pointer to the first array to compare.
 * @param arr2 Pointer to the second array to compare.
 * @param n    Number of items to check.
 * @param size Size of objects in both arrays.
 */
void __assert_array_equals(const void *arr1, const void *arr2, const size_t n, const size_t size);
#define assert_array_equals(arr1, arr2, n, size) __gen_assert__(__assert_array_equals, arr1, arr2, n, size)

/**
 * Assert that two arrays are not equal (that is, they do not contain the same items).
 *
 * PRE: no pointer is null.
 * PRE: both arrays contain objects that are the same size.
 * PRE: both arrays contain at least n items.
 *
 * @param arr1 Pointer to the first array to compare.
 * @param arr2 Pointer to the second array to compare.
 * @param n    Number of items to check.
 * @param size Size of objects in both arrays.
 */
void __assert_array_not_equals(const void *arr1, const void *arr2, const size_t n, const size_t size);
#define assert_array_not_equals(arr1, arr2, n, size) __gen_assert__(__assert_array_not_equals, arr1, arr2, n, size)

/**
 * Assert that a pointer is not null.
 *
 * @param ptr Pointer to check.
 */
void __assert_not_null(const void *ptr);
#define assert_not_null(ptr) __gen_assert__(__assert_not_null, ptr)

/**
 * Assert that a pointer is null.
 *
 * @param ptr Pointer to check.
 */
void __assert_null(const void *ptr);
#define assert_null(ptr) __gen_assert__(__assert_null, ptr)

#endif  // __TESTSUITE_H__

