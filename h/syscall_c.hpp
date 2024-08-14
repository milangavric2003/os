#ifndef PROJECT_BASE_V1_1_SYSCALL_HPP
#define PROJECT_BASE_V1_1_SYSCALL_HPP

#include "../lib/hw.h"
#include "tcb.hpp"
#include "Semaphore.hpp"

static uint64 constexpr MEM_ALLOC_CODE = 0x1;
static uint64 constexpr MEM_FREE_CODE = 0x2;
static uint64 constexpr THREAD_CREATE_CODE = 0x11;
static uint64 constexpr THREAD_EXIT_CODE = 0x12;
static uint64 constexpr THREAD_DISPATCH_CODE = 0x13;

static uint64 constexpr SEM_OPEN_CODE = 0x21;
static uint64 constexpr SEM_CLOSE_CODE = 0x22;
static uint64 constexpr SEM_WAIT_CODE = 0x23;
static uint64 constexpr SEM_SIGNAL_CODE = 0x24;

extern void* mem_alloc (size_t size);

extern int mem_free (void*);

typedef TCB* thread_t;

extern int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg);

extern int thread_exit();

extern void thread_dispatch();

typedef Semaphore* sem_t;

extern int sem_open (sem_t* handle, unsigned init);

extern int sem_close (sem_t handle);

extern int sem_wait (sem_t id);

extern int sem_signal (sem_t id);


#endif