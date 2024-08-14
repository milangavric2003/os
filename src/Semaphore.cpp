#include "../h/Semaphore.hpp"

Semaphore::~Semaphore() {
    while (blocked.peekFirst() != nullptr) {
        TCB* t = blocked.removeFirst();
        Scheduler::put(t);
        ret = -1; // wait() will return this error code
    }
}

int Semaphore::wait () {
    if (--val < 0) block();
    return ret;
}

void Semaphore::signal() {
    if (++val <= 0) unblock();
}

void Semaphore::block() {
    TCB *old = TCB::running;
    blocked.addLast(old);
    TCB::running = Scheduler::get();
    TCB::contextSwitch(&old->context, &TCB::running->context);
}

void Semaphore::unblock() {
    TCB* t = blocked.removeFirst();
    Scheduler::put(t);
}