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
    if (--val < 0) {
        //TCB* volatile running = TCB::running;
        int ret1 = 0; // so I can firstly set sem = nullptr, timedWaitWaken = 0
        //TCB::running->semTimedWait = this;
        TCB::setSemaphore(this); // So that compiler wouldn't make optimization
        TCB::time_sleep(timeout);

        if (TCB::running->timedWaitTimerWaken) {
            val++;
            ret1 = TIMEOUT;
        }
        TCB::running->semTimedWait = nullptr;
        TCB::running->timedWaitTimerWaken = false;
        if (ret == SEMDEAD) ret1 = SEMDEAD;
        return ret1;
    }
    return ret;
}

int SemaphorePomocni::signal() {
    if (ret != 0) return ret;
    if (++val <= 0) {
        if (blocked.peekFirst() != nullptr) unblock();
        else if (TCB::blockedList.peekFirst()) {
            TCB::blockedList.resetTemp();
            TCB* newElem = nullptr;
            for (TCB* elem = TCB::blockedList.peekFirst(); elem != nullptr; ) {
                newElem = TCB::blockedList.peekNext();
                if (elem->semTimedWait == this) {
                    if (newElem) {
                        newElem->timeSleepCounter += elem->timeSleepCounter;
                    }
                    TCB::blockedList.remove(elem);
                    elem->blocked = false;
                    Scheduler::put(elem);

                }
                elem = newElem;
            }
        }
        else return SIGNAL_ERROR;
    }
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

/*
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




 */


