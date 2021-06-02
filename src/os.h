#pragma once

#include "language.h"

i32 GetSystemPageSize(void);

void *MemReserve(size_t size);
void MemCommit(void *ptr, size_t size);
void MemFree(void *ptr, size_t size);
