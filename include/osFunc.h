#pragma once
#include <stddef.h>

typedef enum
{
    ACS_READ = 0,
    ACS_WRITE,
    ACS_ALL
} MappedAccess;

typedef struct
{
    char* memory;
    union
    {
        void* handle; // Only used when compiled for windows
        size_t memLen; // Only used when compiled for linux
    };
    MappedAccess access;
} MappedMemory;

MappedMemory MapFile(const char* filepath, MappedAccess access);
void UnmapFile(MappedMemory* memory);