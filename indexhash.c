#include "indexhash.h"
#include "engine.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Compute bucket index */
unsigned int hash_index(const char *name, size_t bucket_count)
{
    unsigned int h = 0;
    while (*name)
        h = h * 31 + (unsigned char)*name++;
    return h % bucket_count;
}

/* Allocate empty table */
indextable *hash_create(uint32_t bucket_count)
{
    indextable *table = calloc(1, sizeof(indextable));
    if (!table) return NULL;

    table->bucket_count = bucket_count;
    table->bucket = calloc(bucket_count, sizeof(indexnode*));
    if (!table->bucket) { free(table); return NULL; }

    return table;
}

/* Insert process into hash */
int insert_index(const char *name, engine *e, uint64_t record_index)
{
    if (!e || !name) return -1;

    uint64_t idx = hash_index(name, e->index->bucket_count);

    indexnode *new_node = malloc(sizeof(indexnode));
    if (!new_node) return -1;

    strncpy(new_node->name, name, sizeof(new_node->name) - 1);
    new_node->name[sizeof(new_node->name) - 1] = '\0';
    new_node->record_count = record_index;

    /* Insert at head for O(1) insertion */
    new_node->next = e->index->bucket[idx];
    e->index->bucket[idx] = new_node;

    return 0;
}

/* Find process in hash */
int find_index(const char *name, engine *e, uint64_t *out_index)
{
    if (!e || !e->index || !out_index || !name) return -1;

    uint64_t idx = hash_index(name, e->index->bucket_count);
    indexnode *n = e->index->bucket[idx];

    while(n)
    {
        if(strcmp(name, n->name) == 0) {
            *out_index = n->record_count;
            return 0;
        }
        n = n->next;
    }
    return -1;
}

/* Remove process from hash */
int remove_index(const char *name, engine *e, uint64_t *out_index)
{
    if(!e || !e->index || !name) return -1;

    uint64_t h = hash_index(name, e->index->bucket_count);
    indexnode *n = e->index->bucket[h];
    indexnode *prev = NULL;

    while(n)
    {
        if(strcmp(name, n->name) == 0)
        {
            if(prev)
                prev->next = n->next;
            else
                e->index->bucket[h] = n->next;

            if(out_index)
                *out_index = n->record_count;

            free(n);
            return 0;
        }
        prev = n;
        n = n->next;
    }
    return -1;
}

/* Free all hash memory */
int destroy_index(engine *e)
{
    if (!e || !e->index) return -1;

    for(size_t i = 0; i < e->index->bucket_count; i++)
    {
        indexnode *n = e->index->bucket[i];
        while(n)
        {
            indexnode *next = n->next;
            free(n);
            n = next;
        }
    }

    free(e->index->bucket);
    free(e->index);
    e->index = NULL;
    return 0;
}