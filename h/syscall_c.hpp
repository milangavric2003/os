#ifndef PROJECT_BASE_V1_1_SYSCALL_HPP
#define PROJECT_BASE_V1_1_SYSCALL_HPP

#include "../lib/hw.h"
#include "tcb.hpp"

static uint64 constexpr MEM_ALLOC_CODE = 0x1;
static uint64 constexpr MEM_FREE_CODE = 0x2;
static uint64 constexpr THREAD_CREATE_CODE = 0x11;
static uint64 constexpr THREAD_EXIT_CODE = 0x12;
static uint64 constexpr THREAD_DISPATCH_CODE = 0x13;

extern void* mem_alloc (size_t size);

extern int mem_free (void*);

typedef TCB* thread_t;

extern int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg);

extern int thread_exit();

extern void thread_dispatch();



#endif