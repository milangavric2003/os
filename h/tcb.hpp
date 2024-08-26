//
// Created by os on 7/26/24.
//

#ifndef PROJECT_BASE_V1_1_TCB_HPP
#define PROJECT_BASE_V1_1_TCB_HPP

#include "../lib/hw.h"
#include "scheduler.hpp"
#include "MemoryAllocator.hpp"

//Thread Control Block
class TCB {
public:

    void* operator new(size_t size) {
        return MemoryAllocator::mem_alloc(size);
    }

    void operator delete(void* ptr) {
        MemoryAllocator::mem_free(ptr);
    }

    void* operator new[](size_t size) {
        return MemoryAllocator::mem_alloc(size);
    }

    void operator delete[](void* ptr) {
        MemoryAllocator::mem_free(ptr);
    }

    ~TCB() { delete[] stack; }

    bool isFinished() const { return finished; }
    void setFinished(bool finished) { TCB::finished = finished; }

    uint64 getTimeSlice() const {
        return timeSlice;
    }

    using Body = void (*)(void*);

    static int createThread(TCB** handle, Body body, void* arg, void* stack_space);

    static void yield();

    static int thread_exit();

    static TCB *running;

    static int time_sleep(time_t time);

private:
    explicit TCB(TCB** handle, Body body, void* arg, void *stack_space, uint64 timeSlice) :
        handle (handle), arg(arg), body(body), stack(body != nullptr ? (uint8*)stack_space : nullptr),
        context({
            body != nullptr ? (uint64) &threadWrapper : 0,
            stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0
        }),
        timeSlice(timeSlice), finished(false),
        blocked(false), timeSleepCounter(0) {

        if (body != nullptr) Scheduler::put(this); //ako body = nullptr onda je to main korutina, ne treba je u sch
    }
    struct Context{
        //treba nam makar sp, a x3 do x31 cemo cuvati na steku
        uint64 ra;
        uint64 sp;
    };
    TCB** handle;
    void* arg;
    Body body;
    uint8 *stack;
    Context context;
    uint64 timeSlice;
    bool finished;

    bool blocked;
    static List<TCB> blockedList;
    time_t timeSleepCounter;
    static void timer_tick();

    friend class Riscv;
    friend class SemaphorePomocni;

    static void threadWrapper();//treba nam jer na kraju body-a nema set finished

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

    static uint64 timeSliceCounter;

    //static uint64 constexpr DEFAULT_STACK_SIZE = 1024; 4096
    //static uint64 constexpr DEFAULT_TIME_SLICE = 2;
};


#endif //PROJECT_BASE_V1_1_TCB_HPP
