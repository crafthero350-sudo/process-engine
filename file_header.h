/*
 * file_header.h
 *
 * File header and database operations for process engine.
 * Responsibilities:
 * - Define file_header structure.
 * - Declare functions to open, read, write, update, commit, and close the database.
 * Notes:
 * - Always use close_file() to safely persist changes.
 */

#ifndef FILE_HEADER_H
#define FILE_HEADER_H

#include <stdio.h>
#include <stdint.h>
#include "processrecord.h"

#define MAGIC 1162757961
#define VERSION 1

typedef struct file_header {
    uint32_t magic;         // Magic number to validate file
    uint32_t version;       // File version
    uint64_t record_count;  // Number of process records in file
    uint64_t date_start;    // Creation timestamp
} file_header;

/* Open or create a database file. Initialize header if new. */
FILE *file_open(const char *path, file_header *hdr);

/* Append a process record to the file and update header. */
int write_file_record(FILE *fb, file_header *hdr, Processrecord *rec);

/* Read all process records into RAM. */
int read_all_file(FILE *fb, uint64_t record_count, Processrecord *out);

/* Update a specific process record at the given index. */
int update_file(FILE *fb, uint64_t index, Processrecord *rec);

/* Commit header changes to file. */
void commit_file(FILE *fb, file_header *hdr);

/* Close the file safely, committing header first. */
void close_file(FILE *fb, file_header *hdr);

#endif
