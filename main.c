/* main.c
*
 * Simple CLI interface to the in-memory process engine.
 * Handles user commands: add, delete, find, view, exit.
 * Loads engine from file at start, flushes changes on exit.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "engine.h"
#include "processrecord.h"

// Reading inputs for each function
void read_string(char *buffer, size_t size)
{
    printf(">");
    if (fgets(buffer, size, stdin)) {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
}

int main()
{
    char name[64];
    char command[64];
    engine *e = engine_create(1000);
    if(!e) {
        printf("Failed to start engine!\n");
        return -1;
    }
    /* Loading from the database to the engine */
    if(engine_load(e,"process.db") != 0)
    {
        printf("Failed to load database!\n");
    }
    // Main loop: read user commands and execute corresponding engine operations
    while(1)
    {
        read_string(command, sizeof(command));

        if(strcmp(command, "exit") == 0)
        {
            engine_flush(e);
            engine_destroy(e);
            break;
        }
        else if(strcmp(command, "add") == 0)
        {
            read_string(name, sizeof(name));
            if(engine_add(e, name) == 0)
                printf("Added process %s\n", name);
            else
                printf("Failed to add process %s\n", name);

        }
        else if(strcmp(command, "delete") == 0)
        {
            read_string(name, sizeof(name));
            if(engine_delete(e, name) == 0)
                printf("Deleted process %s\n", name);
            else
                printf("Failed to delete process %s\n", name);
        }
        else if(strcmp(command, "find") == 0)
        {
            read_string(name, sizeof(name));
            Processrecord *rec = engine_find(e, name);
            if(rec != NULL)
            {
                printf("Name: %s\tPID: %lu\tCPU: %u\tRAM: %u\n",
                       rec->name, rec->pid, rec->cpu, rec->ram);
            }
            else
            {
                printf("Process %s not found\n", name);
            }
        }
        else if(strcmp(command, "view") == 0)
        {
            engine_get(e);
        }
        else
        {
            printf("Unknown command: %s\n", command);
        }
    }
    return 0;
}