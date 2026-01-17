//
// wal.h
// Write-Ahead Log (WAL) structures and functions
// In-memory log for tracking changes to records
// Author: Eliyas
// Date: 10/01/2026
//

#ifndef IN_MEMORY_PROCESS_ENGINE_V2_WAL_H
#define IN_MEMORY_PROCESS_ENGINE_V2_WAL_H

#include <stdint.h>
#include <stddef.h>

// WAL operation types
enum waltype {
    wal_add,     // Record was added
    wal_delete,  // Record was deleted
    wal_update,  // Record was updated
};

// Single WAL entry
typedef struct {
    enum waltype type;      // Type of operation
    uint64_t record_index;  // Index of affected record
} walenter;

/* Initialize a WAL buffer.
 * Allocates memory and sets size=0, capacity=initial.
 */
int wal_init(walenter **wal ,size_t *size ,size_t *capacity);

/* Append a new entry to WAL.
 * Grows buffer if capacity exceeded.
 */
int wal_append(walenter **wal ,size_t *size ,size_t *capacity, enum waltype type, uint64_t record_index);

/* Reset WAL size to 0. Does not free memory. */
void wal_clear(walenter **wal,size_t *size);

/* Free memory used by WAL buffer. */
void wal_free(walenter **wal);

#endif // IN_MEMORY_PROCESS_ENGINE_V2_WAL_H
