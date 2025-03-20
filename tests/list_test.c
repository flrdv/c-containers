//
// Created by Paul on 10.03.25.
//

#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "list.h"

void setUp(void) {}
void tearDown(void) {}

typedef List(uint32_t) ListU32;

ListU32 fill_list(ListU32 list) {
    for (uint32_t i = 0; i < 10; i++)
        LIST_PUSH(list, i);
    return list;
}

ListU32 test_append_(ListU32 list) {
    list = fill_list(list);

    TEST_ASSERT(list.len == 10);
    TEST_ASSERT(list.len <= list.cap);
    TEST_ASSERT_NOT_NULL(list.ptr);

    for (ssize_t i = 0; i < 10; i++)
        TEST_ASSERT(list.ptr[i] == i);

    return list;
}

void test_push_to_empty_list() {
    ListU32 list = {0};
    list = test_append_(list);
    LIST_FREE(list);
}

void test_push_to_preallocd_list() {
    ListU32 list = NEWLIST(uint32_t, 10);
    uint32_t* origptr = list.ptr;
    list = test_append_(list);
    TEST_ASSERT(origptr == list.ptr);
    LIST_FREE(list);
}

void test_pop() {
    ListU32 list = fill_list((ListU32) NEWLIST(uint32_t, 10));
    uint32_t counter = 9;
    while (list.len > 0)
        TEST_ASSERT(LIST_POP(list) == counter--);
    LIST_FREE(list);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_push_to_empty_list);
    RUN_TEST(test_push_to_preallocd_list);
    RUN_TEST(test_pop);
    return UNITY_END();
}
