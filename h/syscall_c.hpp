#ifndef PROJECT_BASE_V1_1_SYSCALL_HPP
#define PROJECT_BASE_V1_1_SYSCALL_HPP

#include "../lib/hw.h"

static uint64 constexpr MEM_ALLOC_CODE = 0x1;
static uint64 constexpr MEM_FREE_CODE = 0x2;

extern void* mem_alloc (size_t size);

extern int mem_free (void*);

#endif