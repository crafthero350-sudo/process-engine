/*
* processrecord.h
 *
 * Defines a process record structure.
 * Responsibilities:
 * - Store process info in RAM and file.
 */

#ifndef PROCESSRECORD_H
#define PROCESSRECORD_H

#include <stdint.h>

typedef struct Processrecord {
    char name[64];      // Process name
    uint64_t pid;       // Process ID
    uint32_t cpu;       // CPU usage %
    uint32_t ram;       // RAM usage %
    int alive;          // Alive flag
} Processrecord;

#endif
