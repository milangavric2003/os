//
// Created by os on 7/26/24.
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/scheduler.hpp"

//extern "C" void pushRegisters();
//moze ovo gore bez name mangling

TCB* TCB::running = nullptr;

void TCB::yield() {
    Riscv::pushRegisters();

    TCB::dispatch();

    Riscv::popRegisters();
}

void TCB::dispatch() {
    TCB *old = running;
    if (!old->isFinished()) {
        Scheduler::put(old);
    }
    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);

}

TCB *TCB::createCoroutine(TCB::Body body){
    return new TCB(body);
}