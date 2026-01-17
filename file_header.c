/*
* file_header.c
 *
 * Implements file operations for process engine database.
 * Responsibilities:
 * - Open, read, write, update, and close process database safely.
 * - Keep file format consistent and prevent data loss.
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "file_header.h"
#include "processrecord.h"

/* Open a database file. Create and initialize header if new. */
FILE *file_open(const char *path, file_header *hdr)
{
    if (!path || !hdr) return NULL;

    FILE *fb = fopen(path, "r+b");
    if (!fb) {
        fb = fopen(path, "w+b");
        if (!fb) return NULL;

        hdr->version = VERSION;
        hdr->magic = MAGIC;
        hdr->record_count = 0;
        hdr->date_start = (uint64_t)time(NULL);

        if (fwrite(hdr, sizeof(file_header), 1, fb) != 1) {
            fclose(fb);
            return NULL;
        }
        fflush(fb);
    } else {
        if (fread(hdr, sizeof(file_header), 1, fb) != 1) {
            fclose(fb);
            return NULL;
        }
        if (hdr->magic != MAGIC || hdr->version != VERSION) {
            fclose(fb);
            return NULL;
        }
    }

    return fb;
}

/* Append a process record and update header in file. */
int write_file_record(FILE *fb, file_header *hdr, Processrecord *rec)
{
    if (!fb || !hdr || !rec) return -1;

    fseek(fb, 0, SEEK_SET);           // Write header first
    if (fwrite(hdr, sizeof(file_header), 1, fb) != 1) return -1;

    fseek(fb, 0, SEEK_END);           // Append record
    if (fwrite(rec, sizeof(Processrecord), 1, fb) != 1) return -1;

    fflush(fb);
    return 0;
}

/* Read all process records from file into RAM. */
int read_all_file(FILE *fb, uint64_t record_count, Processrecord *out)
{
    if (!fb || !out) return -1;

    fseek(fb, sizeof(file_header), SEEK_SET);
    size_t read = fread(out, sizeof(Processrecord), record_count, fb);
    return read == record_count ? 0 : -1;
}

/* Update a specific record at given index. */
int update_file(FILE *fb, uint64_t index, Processrecord *rec)
{
    if (!fb || !rec) return -1;

    if (fseek(fb, sizeof(file_header) + index * sizeof(Processrecord), SEEK_SET) != 0) return -1;
    if (fwrite(rec, sizeof(Processrecord), 1, fb) != 1) return -1;
    fflush(fb);
    return 0;
}

/* Commit header changes to file to ensure consistency. */
void commit_file(FILE *fb, file_header *hdr)
{
    if (!fb || !hdr) return;

    fseek(fb, 0, SEEK_SET);
    fwrite(hdr, sizeof(file_header), 1, fb);
    fflush(fb);
}

/* Close the file safely after committing header. */
void close_file(FILE *fb, file_header *hdr)
{
    if (!fb) return;
    commit_file(fb, hdr);
    fclose(fb);
}
