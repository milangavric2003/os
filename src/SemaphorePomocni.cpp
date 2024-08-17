#include "../h/SemaphorePomocni.hpp"

SemaphorePomocni::~SemaphorePomocni() {
    while (blocked.peekFirst() != nullptr) {
        TCB* t = blocked.removeFirst();
        Scheduler::put(t);
        ret = -1; // wait() will return this error code
    }
}

int SemaphorePomocni::wait () {
    if (--val < 0) block();
    return ret;
}

void SemaphorePomocni::signal() {
    if (++val <= 0) unblock();
}

void SemaphorePomocni::block() {
    TCB *old = TCB::running;
    blocked.addLast(old);
    TCB::running = Scheduler::get();
    TCB::contextSwitch(&old->context, &TCB::running->context);
}

void SemaphorePomocni::unblock() {
    TCB* t = blocked.removeFirst();
    Scheduler::put(t);
}