#include "../../../testsuite/testsuite.h"
#include "../cubestate.h"
#include "../movequeue.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static MovePriorityQueue *test_queue;

static const CubeState TEST_STATE = {
    .data = {
        {
            { RED, RED, RED },
            { RED, RED, RED },
            { RED, RED, RED }
        },
        {
            { GREEN, GREEN, GREEN },
            { GREEN, GREEN, GREEN },
            { GREEN, GREEN, GREEN }
        },
        {
            { BLUE, BLUE, BLUE },
            { BLUE, BLUE, BLUE },
            { BLUE, BLUE, BLUE }
        },
        {
            { YELLOW, YELLOW, YELLOW },
            { YELLOW, YELLOW, YELLOW },
            { YELLOW, YELLOW, YELLOW }
        },
        {
            { ORANGE, ORANGE, ORANGE },
            { ORANGE, ORANGE, ORANGE },
            { ORANGE, ORANGE, ORANGE }
        },
        {
            { WHITE, WHITE, WHITE },
            { WHITE, WHITE, WHITE },
            { WHITE, WHITE, WHITE }
        }
    },
    .history_count = 0u,
    .history = { { .face = TOP, .direction = CW } }
};

static void test_add_to_queue(void) {
    assert_true(add_to_move_priority_queue(test_queue, &TEST_STATE, 5));
    fprintf(stderr, "Count: %zu\n", test_queue->count);
    assert_true(add_to_move_priority_queue(test_queue, &TEST_STATE, 4));
    fprintf(stderr, "Count: %zu\n", test_queue->count);
    assert_true(add_to_move_priority_queue(test_queue, &TEST_STATE, 3));
    fprintf(stderr, "Count: %zu\n", test_queue->count);
    assert_true(add_to_move_priority_queue(test_queue, &TEST_STATE, 2));
    fprintf(stderr, "Count: %zu\n", test_queue->count);
}

static void test_poll_from_queue(void) {
    MoveQueueNode result;

    poll_move_priority_queue(test_queue, &result);

    fprintf(stderr, "Count: %zu\n", test_queue->count);

    assert_sint_equals(MQ_OK, test_queue->error);
    assert_uint_equals(2, result.cost);
}

static void test_queue_underflow(void) {
    MoveQueueNode result = { .state = TEST_STATE, .hash = 0u, .cost = 0u };

    poll_move_priority_queue(test_queue, &result);

    assert_sint_equals(MQ_UNDERFLOW, test_queue->error);
    assert_uint_equals(0u, result.hash);
    assert_uint_equals(0u, result.cost);

    assert_sint_equals(0, memcmp(&result.state, &TEST_STATE, sizeof(CubeState)));
}

static const Test TESTS[3] = {
    { .test = test_add_to_queue, .name = "Adding to queue preserves all items" },
    { .test = test_poll_from_queue, .name = "Polling queue pulls the lowest heuristics first" },
    { .test = test_queue_underflow, .name = "Queue underflow is handled correctly" }
};

int main(void) {
    fprintf(stderr, "--- %s ---\n", __FILE__);
    test_queue = new_move_priority_queue(4);
    assert(test_queue);

    run_tests(TESTS, sizeof(TESTS) / sizeof(Test));

    assert(free_move_priority_queue(test_queue));

    return 0;
}

