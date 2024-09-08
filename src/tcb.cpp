//
// Created by os on 7/26/24.
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"

//extern "C" void pushRegisters();
//moze ovo gore bez name mangling

TCB* TCB::running = nullptr;

uint64 TCB::timeSliceCounter = 0;

List<TCB> TCB::blockedList;

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
    if (!old->isFinished() && !old->blocked) {
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
    TCB::dispatch();
    return 0;
}

int TCB::time_sleep(time_t sleep_duration) {
    if (running->blocked) return -1; // already blocked, must be some error
    if (sleep_duration == 0) return 0; // no need to sleep
    running->blocked = true;

    time_t accumulated_time = 0;

    blockedList.resetTemp();
    for (TCB* elem = blockedList.peekFirst(); elem != nullptr; elem = blockedList.peekNext()) {
        accumulated_time += elem->timeSleepCounter;
        if (accumulated_time > sleep_duration) {
            running->timeSleepCounter = sleep_duration - (accumulated_time - elem->timeSleepCounter);
            elem->timeSleepCounter -= running->timeSleepCounter;
            blockedList.addBefore(running, elem);
            TCB::dispatch();
            return 0;
        } else if (accumulated_time == sleep_duration) {
            // Insert after the current element since they have the same wake-up time
            running->timeSleepCounter = 0;

            // Special case: Insert newElem right after elem
            TCB* nextElem = blockedList.peekNext();
            if (nextElem) {
                blockedList.addBefore(running, nextElem);
            } else {
                // If elem is the last element, just add it to the end
                blockedList.addLast(running);
            }
            TCB::dispatch();
            return 0;
        }
    }

    // If the new element should be the last one
    running->timeSleepCounter = sleep_duration - accumulated_time;
    blockedList.addLast(running);

    TCB::dispatch();
    return 0;
}

void TCB::timer_tick() {
    if (TCB *firstElem = blockedList.peekFirst()) {
        firstElem->timeSleepCounter--;
        while (firstElem && firstElem->timeSleepCounter == 0) {
            blockedList.removeFirst();
            firstElem->blocked = false;
            Scheduler::put(firstElem);
            // If it waits on semaphore (timedwait)
            if (firstElem->semTimedWait) {
                firstElem->timedWaitTimerWaken = true;
            }
            firstElem = blockedList.peekFirst();
        }
    }
}

void TCB::setSemaphore(SemaphorePomocni *newSemaphore) {
    TCB::running->semTimedWait = newSemaphore;
}
