#ifndef PROJECT_BASE_V1_1_SYSCALL_HPP
#define PROJECT_BASE_V1_1_SYSCALL_HPP

#include "../lib/hw.h"


extern void* mem_alloc (size_t size);

extern int mem_free (void*);

#endif