//
// Created by Paul on 10.03.25.
//

#pragma once

#include <stdint.h>

#define KEY uint32_t
#define VALUE uint32_t

static const int bucketCnt = 8;

typedef struct {
    KEY key;
    VALUE value;
} Bucket;

typedef struct {

} Buckets;

typedef struct {
    uint8_t hashbits; // log2 of lower hash bits for buckets addressing
    Bucket*
} Hashmap;
