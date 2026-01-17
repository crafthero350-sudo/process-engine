/*
* engine/h
*
* Memory-backed process engine interface.
*
* Responsibilities:
* - Manages all process in RAM
* - Maintains a hash index for fast name lookups.
* - Tracks changes with a WAL (Write-Ahead Log) for crash recovery.
* - Persists data to disk when needed (engine_save / engine_flush).
*
* Notes / Warnings
* - Improper use or skipping engine_destroy() may cause memory leaks.
* - Skipping engine_flush() before exit may lead to data loss.
* - Any corruption in the WAL or index may cause inconsistent state.
*
* Typical usage:
 * 1. engine_create() to initialize engine.
 * 2. engine_load() to load existing database.
 * 3. engine_add / engine_delete to modify processes.
 * 4. engine_flush / engine_save to persist changes.
 * 5. engine_destroy() to free resources.
 */

#ifndef ENGINE_H
#define ENGINE_H
#include <stdio.h>
#include <stddef.h>
#include "processrecord.h"
#include "file_header.h"
#include "wal.h"
#define MAX_RECORDS 100000

struct indextable;
typedef struct indextable indextable;

/*
 * engine
 *
 * Operations management and linking it to the file
 *
 *  Responsibilities
 * - Track all operations in memory and modify its state
 * - Index management to speed up search and access to operations
 * - Recording changes to ensure they can be retrieved (WAL)
 */
typedef struct engine {
    FILE *fb;
    file_header hdr;

    Processrecord *process; // pointer to Processsrecord.h
    size_t count; // Used operations
    size_t capacity; // Capacity process

    indextable *index;

    walenter *wal;
    size_t wal_size; // Used operations
    size_t wal_capacity; // Capacity wal

    int dirty; // dirty = 1 There are changes dirty = 0 No changes
} engine;
/**/
engine *engine_create(size_t init_capacity);
int engine_load(engine *e, const char *path);
void engine_destroy(engine *e);
Processrecord *engine_get(engine *e);
int engine_add(engine *e, const char *name);
Processrecord *engine_find(engine *e, const char *name);
int engine_delete(engine *e, const char *name);
int engine_flush(engine *e);
int engine_save(engine *e);
#endif
