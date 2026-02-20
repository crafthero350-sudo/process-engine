/*
* engine.c
 *
 * Implements engine functions.
 */

#include "engine.h"
#include "indexhash.h"
#include <stdlib.h>
#include <string.h>


/* Allocate engine, process array, index, WAL */
engine *engine_create(size_t init_capacity)
{
    if (init_capacity >= MAX_RECORDS) {printf("Memory is fill");return NULL;}
    engine *e = calloc(1, sizeof(engine));
    if (!e) return NULL;

    e->capacity = init_capacity;
    e->process = calloc(init_capacity, sizeof(Processrecord));
    if (!e->process) goto fail;
    e->index = hash_create(init_capacity);

    if (wal_init(&e->wal, &e->wal_size, &e->wal_capacity) != 0)
        goto fail;

    return e;
    fail:{
        engine_destroy(e);
        return NULL;
    }
}
/* Load file and rebuild index */
int engine_load(engine *e, const char *path)
{
    if (!e || !path) return -1;
    e->fb = file_open(path, &e->hdr);
    if (!e->fb) return -1;
    if (read_all_file(e->fb,e->hdr.record_count,e->process)!= 0) {close_file(e->fb, &e->hdr);return -1;}
    if (e->hdr.record_count > 0)
    {
        e->count = e->hdr.record_count;
        for (uint64_t i = 0; i < e->count; i++)
        {
            if (e->process[i].alive)
            {
                insert_index(e->process[i].name,e,i);
            }
        }
    }
    return 0;
}
/* Free all resources and close file */
void engine_destroy(engine *e)
{
    if (!e) return;
    if (e->fb) close_file(e->fb, &e->hdr);
    if (e->index) destroy_index(e);
    wal_free(&e->wal);
    free(e->process);
    free(e);
}

/* Add a new process in RAM and WAL */
int engine_add(engine *e, const char *name)
{
    if (e->capacity == e->count) return -1;
    Processrecord *r = &e->process[e->count];
    r->pid = e->count;
    strncpy(r->name, name, sizeof(r->name));
    r->name[sizeof(r->name)-1] = '\0';
    r->cpu = rand() %60;
    r->ram = rand() %80;
    r->alive = 1;

    insert_index(name, e, r->pid);
    wal_append(&e->wal, &e->wal_size, &e->wal_capacity, wal_add, r->pid);

    e->count++;
    e->dirty = 1;
    return 0;
}
/* Find process in RAM using index */
Processrecord *engine_find(engine *e, const char *name)
{
    uint64_t idx;
    if (find_index(name, e, &idx) != 0) return NULL;
    return &e->process[idx];
}

/* Logical delete process and update WAL */
int engine_delete(engine *e, const char *name)
{
    uint64_t idx;
    if (find_index(name, e, &idx) != 0) return -1;

    e->process[idx].alive = 0;
    remove_index(name, e,&idx);
    wal_append(&e->wal, &e->wal_size, &e->wal_capacity, wal_delete, idx);
    e->dirty = 1;
    return 0;
}

/* Print all alive processes */
Processrecord *engine_get(engine *e)
{
    if (!e) return NULL;
    if (e->count == 0)  {printf("No process\n");return NULL;}
    for (int i = 0; i < e->count; i++)
    {
        Processrecord *rec = &e->process[i];
        if (rec->alive)
            printf("Name: %s\tPID: %lu\tCPU: %u\tRAM: %u\n",rec->name,rec->pid,
                rec->cpu, rec->ram);
    }
    return e->process;
}

/* Flush changes to disk if dirty */
int engine_flush(engine *e)
{
    if (e->dirty) engine_save(e);
    return 0;
}

/* Save engine state to file */
int engine_save(engine *e)
{
    if (!e || !e->fb || !e->process) return -1;

    fseek(e->fb, sizeof(file_header), SEEK_SET); // تجاوز header
    for(size_t i = 0; i < e->count; i++)
    {
        if(fwrite(&e->process[i], sizeof(Processrecord), 1, e->fb) != 1)
            return -1;
    }
    fflush(e->fb);

    e->hdr.record_count = e->count;
    commit_file(e->fb, &e->hdr);
    e->dirty = 0;
    return 0;

}
