#include "../h/SemaphorePomocni.hpp"

int SemaphorePomocni::wait () {
    if (ret != 0) return ret;
    if (--val < 0) block();
    return ret;
}

int SemaphorePomocni::trywait() {
    if (ret != 0) return ret;
    if (--val < 0) return 1;
    else return 0;
}

int SemaphorePomocni::timedwait(time_t timeout) {
    if (ret != 0) return ret;
    if (--val < 0) TCB::time_sleep(timeout);
    return ret;
}

int SemaphorePomocni::signal() {
    if (ret != 0) return ret;
    if (++val <= 0) unblock();
    return ret;
}

void SemaphorePomocni::block() {
    TCB *old = TCB::running;
    blocked.addLast(old);
    TCB::running = Scheduler::get();
    TCB::contextSwitch(&old->context, &TCB::running->context);
}

void SemaphorePomocni::unblock() {
    TCB* t = blocked.removeFirst(); // t can be null if blocked list is empty
    Scheduler::put(t);
}

int SemaphorePomocni::close() {
    if (ret != 0) return ret;
    while (blocked.peekFirst() != nullptr) {
        TCB* t = blocked.removeFirst();
        Scheduler::put(t);
    }
    ret = SEMDEAD; // wait() will return this error code
    return 0;
}



