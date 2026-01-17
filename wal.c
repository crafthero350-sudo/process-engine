//
// wal.c
//
// Write-Ahead Log (WAL) implementation.
//
// Responsibilities:
// - Keep track of engine operations (add / delete / update) before flushing to disk.
// - Preserve consistency in case of crashes.
// - Manage memory for WAL entries.
//
#include <stdint.h>
#include <stdlib.h>
#include "wal.h"

/*
 * Initialize WAL buffer.
 * Allocates memory, sets size = 0, capacity = 8.
 * Caller must call wal_free() to release memory.
 */
int wal_init(walenter **wal ,size_t *size ,size_t *capacity)
{
    if (!wal || !size || !capacity)
        return -1;

    // Initial capacity chosen to reduce frequent reallocations
    *capacity = 8;

    // WAL starts empty after initialization
    *size = 0;

    // Allocate contiguous memory for WAL entries
    *wal = malloc(sizeof(walenter) * (*capacity));
    if (!*wal)
        return -1;

    return 0;
}


/*
 * Append a new entry to WAL.
 * Automatically grows buffer if capacity exceeded.
 * Logs operation type and affected record index.
 */
int wal_append(walenter **wal ,size_t *size ,size_t *capacity, enum waltype type, uint64_t record_index)
{
    if (!wal || !size || !capacity)
        return -1;

    // Grow WAL buffer when capacity is exhausted
    if (*size >= *capacity)
    {
        size_t new_capacity = (*capacity) * 2;

        // Reallocate WAL buffer with larger capacity
        walenter *temp =
            realloc(*wal, sizeof(walenter) * new_capacity);
        if (!temp)
            return -1;

        *wal = temp;
        *capacity = new_capacity;
    }

    // Write WAL entry at the next free slot
    (*wal)[*size].type = type;
    (*wal)[*size].record_index = record_index;

    // Advance WAL size after successful append
    (*size)++;

    return 0;
}

/*
 * Clear WAL logically.
 * Resets wal_size to 0. Memory is not freed.
 */
void wal_clear(walenter **wal,size_t *wal_size)
{
    if (!wal || !*wal || !wal_size)
        *wal_size = 0;
}

/*
 * Free WAL memory.
 * Releases buffer allocated by wal_init().
 * Pointer must not be used after this.
 */
void wal_free(walenter **wal)
{
    if (wal)
        free(wal);
}