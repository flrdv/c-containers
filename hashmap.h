//
// Created by Paul on 10.03.25.
//

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "list.h"

#define BUCKET_SLOTS 8

static const int minB = 3;
static const int minBucketCnt = 1 << minB;
// start growing with an average of 7 slots taken over all buckets
static const int loadfactor = 6;

// tophash_empty and tophash_empty_rest are almost the same, just the tophash_empty_rest
// hints lookup to stop looking for a value and report no-entry.
static const uint8_t tophash_empty_rest = 0;
static const uint8_t tophash_empty = 1;
static const uint8_t tophash_min = 2;

// these are temporary and used to distinguish key-value types wherever they are supposed to be
// replaced by a generic
#define KEY int64_t
#define VALUE int8_t

// TODO: make it platform-dependent. Therefore it should be uint64_t on 64-bit
#define HASHTYPE uint32_t

struct Bucket;

typedef struct Bucket {
    uint8_t tophash[BUCKET_SLOTS];
    KEY keys[BUCKET_SLOTS];
    VALUE values[BUCKET_SLOTS];
    struct Bucket* nextb;
} Bucket;

typedef struct {
    uint8_t b;          // log2 of lower hash bits for buckets addressing
    uint32_t len;       // a number of slots taken
    Bucket* buckets;
    List(Bucket*) overflowbuckets;
} Hashmap;

typedef struct {
    Hashmap* map;
    uint32_t currbucket;
    uint32_t currelem;
    Bucket* curroverflow;
} HMIter;

static HASHTYPE map_hash(const KEY key) {
    return key;
}

static uint8_t tophash(const HASHTYPE hash) {
    uint8_t th = hash >> 24;
    if (th < tophash_min)
        th += tophash_min;
    return th;
}

static uint32_t bucketMask(const uint8_t b) {
    return (1 << b) - 1;
}

static Bucket map_newbucket() {
    // 0 here means tophash_empty_rest
    return (Bucket){.tophash = {0}};
}

static Bucket* map_getoverflow(Hashmap* map) {
    if (map->overflowbuckets.len == 0) {
        // TODO: we could store buckets in a list instead of malloc'ing every single one
        Bucket* newbucket = malloc(sizeof(Bucket));
        *newbucket = map_newbucket();
        return newbucket;
    }

    Bucket* bucket = LIST_POP(map->overflowbuckets);
    *bucket = map_newbucket();

    return bucket;
}

static void map_returnoverflow(Hashmap* map, Bucket* bucket) {
    LIST_PUSH(map->overflowbuckets, bucket);
}

static Bucket* map_newbucketarray(const uint8_t b) {
    Bucket* buckarr = calloc(1 << b,  sizeof(Bucket));
    return buckarr;
}

static Hashmap map_new() {
    return (Hashmap){
        .b = minB,
        .len = 0,
        .buckets = map_newbucketarray(minB),
        .overflowbuckets = NEWLIST(Bucket*, 0),
    };
}

static VALUE map_access(const Hashmap* map, const KEY key) {
    const HASHTYPE hash = map_hash(key);
    const uint8_t th = tophash(hash);
    const uint32_t buckindex = hash & bucketMask(map->b);
    Bucket bucket = map->buckets[buckindex];

    for (;;) {
        for (ssize_t i = 0; i < BUCKET_SLOTS; i++) {
            if (bucket.tophash[i] == tophash_empty_rest) {
                // NO ENTRY. TODO: add lookup failure bool mark.
                return 0;
            }

            if (bucket.tophash[i] == th && bucket.keys[i] == key) {
                return bucket.values[i];
            }
        }

        if (bucket.nextb == NULL) {
            // NO ENTRY. TODO: Add later returning bool.
            return 0;
        }

        bucket = *bucket.nextb;
    }
}

static uint32_t map_totalbuckets(const Hashmap* map) {
    return (1 << map->b) + map->overflowbuckets.len;
}

static bool map_shouldgrow(const Hashmap* map) {
    return map->len / map_totalbuckets(map) > loadfactor;
}

static void map_insert_(Hashmap* map, KEY key, VALUE value);

static void map_insert_from_bucket_(Hashmap* map, const Bucket* bucket) {
    for (ssize_t slot = 0; slot < BUCKET_SLOTS; slot++) {
        if (bucket->tophash[slot] == tophash_empty)
            continue;
        if (bucket->tophash[slot] == tophash_empty_rest)
            break;

        map_insert_(map, bucket->keys[slot], bucket->values[slot]);
    }
}

static void map_grow(Hashmap* map) {
    map->b++;
    Bucket* oldbuckets = map->buckets;
    map->buckets = map_newbucketarray(map->b);

    for (ssize_t i = 0; i < bucketMask(map->b-1)+1; i++) {
        const Bucket* bucket = &oldbuckets[i];
        map_insert_from_bucket_(map, bucket);

        Bucket* overflow = bucket->nextb;

        while (overflow != NULL) {
            map_insert_from_bucket_(map, overflow);
            map_returnoverflow(map, overflow);
            overflow = overflow->nextb;
        }
    }

    free(oldbuckets);
}

static void map_insert_(Hashmap* map, const KEY key, const VALUE value) {
    const HASHTYPE hash = map_hash(key);
    const uint32_t buckindex = hash & bucketMask(map->b);
    Bucket* bucket = &map->buckets[buckindex];

    for (;;) {
        for (ssize_t i = 0; i < BUCKET_SLOTS; i++) {
            if (bucket->tophash[i] >= tophash_min)
                continue;

            bucket->tophash[i] = tophash(hash);
            bucket->keys[i] = key;
            bucket->values[i] = value;
            map->len++;
            return;
        }

        if (bucket->nextb == NULL)
            bucket->nextb = map_getoverflow(map);

        bucket = bucket->nextb;
    }
}

static void map_insert(Hashmap* map, const KEY key, const VALUE value) {
    if (map_shouldgrow(map))
        map_grow(map);

    map_insert_(map, key, value);
}

static void map_free(Hashmap const* map) {
    for (ssize_t i = 0; i < map->overflowbuckets.len; i++)
        free(map->overflowbuckets.ptr[i]);

    LIST_FREE(map->overflowbuckets);
    free(map->buckets);
}
