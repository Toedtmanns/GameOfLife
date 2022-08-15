#include "osFunc.h"
#include <stdio.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winerror.h>

MappedMemory MapFile(const char* filepath, MappedAccess access)
{
    MappedMemory memory;
    memory.access = access;

    DWORD desAccess;
    switch (access)
    {
    case ACS_READ:
        desAccess = FILE_MAP_READ;
        break;
    case ACS_WRITE:
        desAccess = FILE_MAP_WRITE;
        break;
    case ACS_ALL:
        desAccess = FILE_MAP_ALL_ACCESS;
        break;
    }

    UINT fileAccess;
    switch (access)
    {
    case ACS_READ:
        fileAccess = OF_READ;
        break;
    case ACS_WRITE:
        fileAccess = OF_WRITE;
        break;
    case ACS_ALL:
        fileAccess = OF_READWRITE;
        break;
    }

    DWORD pageAccess;
    switch (access)
    {
    case ACS_READ:
        pageAccess = PAGE_READONLY;
        break;
    case ACS_WRITE:
        pageAccess = PAGE_READWRITE;
        break;
    case ACS_ALL:
        pageAccess = PAGE_READWRITE;
        break;
    }

    OFSTRUCT ofStruct;
    HFILE file = OpenFile(
        filepath,
        &ofStruct,
        fileAccess
    );

    memory.handle = CreateFileMappingA(
        file,
        NULL,
        pageAccess,
        0,
        0,
        NULL
    );

    if (memory.handle == 0)
    {
        printf("Could not map file: %d\n",  GetLastError());
        return memory;
    }

    memory.memory = MapViewOfFile(
        memory.handle,
        desAccess,
        0,
        0,
        0
    );

    CloseHandle(file);

    return memory;
}
void UnmapFile(MappedMemory* memory)
{
    UnmapViewOfFile(memory->memory);
    CloseHandle(memory->handle);
    memory->memory = NULL;
    memory->handle = NULL;
}

#else
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

MappedMemory MapFile(const char* filepath, MappedAccess access)
{
    int fileAccess;
    switch (access)
    {
    case ACS_READ:
        fileAccess = O_RDONLY;
        break;
    case ACS_WRITE:
        fileAccess = O_WRONLY;
        break;
    case ACS_ALL:
        fileAccess = O_RDWR;
        break;
    }

    int pageAccess;
    switch (access)
    {
    case ACS_READ:
        pageAccess = PROT_READ;
        break;
    case ACS_WRITE:
        pageAccess = PROT_WRITE;
        break;
    case ACS_ALL:
        pageAccess = PROT_READ | PROT_WRITE;
        break;
    }

    MappedMemory memory;
    memory.access = access;

    int fd = open(filepath, access);

    if (fd == -1)
    {
        printf("File '%s' couldn't be opened!\n", filepath);
        return memory;
    }

    struct stat statbuf;
    fstat(fd, &statbuf);
    memory.memory = mmap(NULL, statbuf.st_size, pageAccess, MAP_SHARED, fd, 0);

    close(fd);

    return memory;
}
void UnmapFile(MappedMemory* memory)
{
    munmap(memory->memory, memory->memLen);
}

#endif //_WIN32