//
// Created by os on 7/26/24.
//

#ifndef PROJECT_BASE_V1_1_TCB_HPP
#define PROJECT_BASE_V1_1_TCB_HPP

#include "../lib/hw.h"
#include "scheduler.hpp"

//Thread Control Block
class TCB {
public:

    ~TCB() { delete[] stack; }

    bool isFinished() const { return finished; }
    void setFinished(bool finished) { TCB::finished = finished; }

    uint64 getTimeSlice() const {
        return timeSlice;
    }

    using Body = void (*)();

    static TCB *createThread(Body body);

    static void yield();

    static TCB *running;

private:
    TCB(Body body, uint64 timeSlice) : body(body), stack(body != nullptr ? new uint64[STACK_SIZE] : nullptr),
        context({
            body != nullptr ? (uint64) &threadWrapper : 0,
            stack != nullptr ? (uint64) &stack[STACK_SIZE] : 0
        }),
        timeSlice(timeSlice),
        finished(false){
        if (body != nullptr) Scheduler::put(this); //ako body = nullptr onda je to main korutina, ne treba je u sch
    }
    struct Context{
        //treba nam makar sp, a x3 do x31 cemo cuvati na steku
        uint64 ra;
        uint64 sp;
    };
    Body body;
    uint64 *stack;
    Context context;
    uint64 timeSlice;
    bool finished;

    friend class Riscv;

    static void threadWrapper();//treba nam jer na kraju body-a nema set finished

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

    static uint64 timeSliceCounter;

    static uint64 constexpr STACK_SIZE = 1024;
    static uint64 constexpr TIME_SLICE = 2;
};


#endif //PROJECT_BASE_V1_1_TCB_HPP
