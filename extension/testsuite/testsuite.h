#ifndef __TESTSUITE_H__
#define __TESTSUITE_H__

#include <stdbool.h>
#include <stdlib.h>

/**
 * Test functions are essentially void functions that take no arguments.
 *
 * They usually will contain at least one assert.
 */
typedef void (*TestFunction)(void);

/**
 * A pair struct holding a test function and the test's name.
 * E.g. test: test_ints_equal | name: "test if two returned ints are equal"
 */
typedef struct {
    TestFunction test;  /**< Pointer to test function. */
    char *name;         /**< Name or objective of function. */
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

/**
 * Assert that a condition holds.
 *
 * @param condition The condition to check.
 */
void assert_true(const bool condition);

/**
 * Assert that a condition does not hold.
 *
 * @param condition The condition to check.
 */
void assert_false(const bool condition);

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
void assert_equals(const void *obj1, const void *obj2, const size_t size);

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
void assert_not_equals(const void *obj1, const void *obj2, const size_t size);

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
void assert_array_equals(const void *arr1, const void *arr2, const size_t n, const size_t size);

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
void assert_array_not_equals(const void *arr1, const void *arr2, const size_t n, const size_t size);

/**
 * Assert that a pointer is not null.
 *
 * @param ptr Pointer to check.
 */
void assert_not_null(const void *ptr);

/**
 * Assert that a pointer is null.
 *
 * @param ptr Pointer to check.
 */
void assert_null(const void *ptr);

#endif  // __TESTSUITE_H__

