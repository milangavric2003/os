//
// Created by os on 7/26/24.
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/scheduler.hpp"

//extern "C" void pushRegisters();
//moze ovo gore bez name mangling

TCB* TCB::running = nullptr;

uint64 TCB::timeSliceCounter = 0;

void TCB::yield() {
    /*Riscv::pushRegisters();

    TCB::dispatch();

    Riscv::popRegisters();*///ne treba jer vise ne menjamo kontekst na ovaj nacin

    __asm__ volatile ("ecall");//ovo vodi u prekidnu rutinu
}

void TCB::dispatch() {
    TCB *old = running;
    if (!old->isFinished()) {
        Scheduler::put(old);
    }
    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);

}

TCB *TCB::createThread(TCB::Body body){
    return new TCB(body, DEFAULT_TIME_SLICE);
}

void TCB::threadWrapper() {
    //ovde bismo trebali da kazemo da izlazimo iz privilegovanog rezima iako smo jos uvek u kodu prekidne rutine
    //spie i spp treba da vrati na stanje pre ulaska u prek. rutinu
    Riscv::popSppSpie();
    running->body();
    running->setFinished(true);
    TCB::yield();
}