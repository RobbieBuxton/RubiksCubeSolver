#include "../../../testsuite/testsuite.h"
#include "../cubestate.h"
#include "../movequeue.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static MovePriorityQueue *test_queue;

static const CubeState TEST_STATE = {
    .data = { { { RED } } },
    .history_count = 0u,
    .history = { { .face = TOP, .direction = CW } }
};

static void test_add_to_queue(void) {
    assert_true(add_to_move_priority_queue(test_queue, &TEST_STATE, 4));
    assert_true(add_to_move_priority_queue(test_queue, &TEST_STATE, 2));
    assert_true(add_to_move_priority_queue(test_queue, &TEST_STATE, 5));
    assert_true(add_to_move_priority_queue(test_queue, &TEST_STATE, 3));
}

static void test_poll_from_queue(void) {
    MoveQueueNode result;
    uint64_t polls[4] = { 0 };

    for (int i = 0; i < 4; ++i) {
        poll_move_priority_queue(test_queue, &result);
        assert_sint_equals(MQ_OK, test_queue->error);
        polls[i] = result.heuristic_value;
    }

    fprintf(stderr, "GOT: { %lu, %lu, %lu, %lu }.\n", polls[0], polls[1], polls[2], polls[3]);

    assert_uint_equals(2, polls[0]);
    assert_uint_equals(3, polls[1]);
    assert_uint_equals(4, polls[2]);
    assert_uint_equals(5, polls[3]);
}

static void test_queue_underflow(void) {
    MoveQueueNode result = { .state = TEST_STATE, .hash = 0u, .heuristic_value = 0u };

    poll_move_priority_queue(test_queue, &result);

    assert_sint_equals(MQ_UNDERFLOW, test_queue->error);
    assert_uint_equals(0u, result.hash);
    assert_uint_equals(0u, result.heuristic_value);

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

