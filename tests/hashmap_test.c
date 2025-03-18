//
// Created by Paul on 10.03.25.
//

#include "unity.h"

#include "hashmap.h"

void setUp(void) {}
void tearDown(void) {}

void test_lookup_empty_map() {
    const Hashmap map = map_new();
    for (int64_t i = 0; i < 10; i++) {
        VALUE value = map_access(&map, i);
        TEST_ASSERT(value == 0);
    }
    map_free(&map);
}

void test_insert() {
    Hashmap map = map_new();
    for (int64_t i = 0; i < 10; i++) {
        map_insert(&map, i, (VALUE)i);
    }
    for (int64_t i = 0; i < 10; i++) {
        VALUE value = map_access(&map, i);
        TEST_ASSERT_EQUAL((VALUE)i, value);
    }
    map_free(&map);
}

void test_grow() {
    Hashmap map = map_new();
    const int values = 1000000;

    for (int64_t i = 1; i < values+1; i++)
        map_insert(&map, i, (VALUE)(i^123));

    for (int64_t i = 1; i < values+1; i++) {
        VALUE value = map_access(&map, i);
        TEST_ASSERT_EQUAL((VALUE)(i^123), value);
    }

    map_free(&map);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_lookup_empty_map);
    RUN_TEST(test_insert);
    RUN_TEST(test_grow);
    return UNITY_END();
}
