/*
* indexhash.h
 *
 * Hash table for process lookup.
 */

#ifndef INDEXHASH_H
#define INDEXHASH_H

#include "engine.h"
#include <stdint.h>

typedef struct indexnode {
    char name[64];
    uint64_t record_count;
    struct indexnode *next;
} indexnode;

typedef struct indextable {
    uint32_t bucket_count;
    indexnode **bucket;
} indextable;

unsigned int hash_index(const char *name, size_t bucket_count);
indextable *hash_create(uint32_t bucket_count);
int insert_index(const char *name, engine *e, uint64_t record_index);
int find_index(const char *name, engine *e, uint64_t *out_index);
int remove_index(const char *name, engine *e, uint64_t *out_index);
int destroy_index(engine *e);

#endif
