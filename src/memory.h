#pragma once

#include "language.h"

typedef struct MemoryArena MemoryArena;
struct MemoryArena
{
    u8 *buffer;
    u8 *allocd;
    u8 *committed;
    u8 *end;
};

MemoryArena CreateArena(size_t capacity);
void *ArenaAlloc(MemoryArena *a, size_t size, size_t align);
void ArenaReset(MemoryArena *a);
void ArenaFree(MemoryArena *a);

//

void InitMemoryArenas(void);

void *PermanentAlloc(size_t size);
void *PermanentAllocAlign(size_t size, size_t align);

void *ScratchAlloc(size_t size);
void *ScratchAllocAlign(size_t size, size_t align);
void ScratchReset(void);

//

typedef struct ArenaSavePoint ArenaSavePoint;
struct ArenaSavePoint
{
    MemoryArena *arena;
    u8 *saved;
};

ArenaSavePoint ArenaSave(MemoryArena *a);
void ArenaRestore(ArenaSavePoint s);
