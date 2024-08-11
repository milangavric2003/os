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

    __asm__ volatile ("mv a0, %[THREAD_DISPATCH_CODE]" : : [THREAD_DISPATCH_CODE] "r"(THREAD_DISPATCH_CODE));
    // a0 <= THREAD_CREATE_CODE
    __asm__ volatile ("ecall");//ovo vodi u prekidnu rutinu
}

void TCB::dispatch() {
    TCB *old = running;
    if (!old->isFinished()) {
        Scheduler::put(old);
    }
    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);
    // ovde se ne dolazi ako novoizabrana nit -> ona krece sa threadWprapper
}

int TCB::createThread(TCB** handle, Body body, void* arg, void* stack_space){
    if ((*handle = new TCB(handle, body, arg, stack_space, DEFAULT_TIME_SLICE)) == nullptr) {
        return -1;
    } else return 0;
}

void TCB::threadWrapper() {
    //ovde bismo trebali da kazemo da izlazimo iz privilegovanog rezima iako smo jos uvek u kodu prekidne rutine
    //spie i spp treba da vrati na stanje pre ulaska u prek. rutinu
    Riscv::popSppSpie(); // jer ova nit u ovom trenutku nije usla u prekid vec je samo izabrana
    running->body(running->arg);
    running->setFinished(true);
    TCB::yield();
}

int TCB::thread_exit() {
    running->setFinished(true);
    //yield
    return 0;
}