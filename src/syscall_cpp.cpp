#include "../h/syscall_cpp.hpp"

Thread::Thread (void (*body)(void*), void* arg) : myHandle(nullptr), body(body), arg(arg) {}

Thread::Thread () : myHandle(nullptr), body(nullptr), arg(nullptr) {}

Thread::~Thread() {
    delete myHandle;
}

int Thread::start () {
    if (myHandle == nullptr) {
        if (body == nullptr) {
            return thread_create(&myHandle, &starter, this);
        } else {
            return thread_create(&myHandle, body, arg);
        }
    }
    return -5; // already called start() on thread
}

void Thread::starter(void* toStart) {
    Thread* t = (Thread*) toStart;
    if (t) t->run();
}

void Thread::dispatch () {
    thread_dispatch();
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}


Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

int Semaphore::timedWait(time_t time) {
    return sem_timedwait(myHandle, time);
}

int Semaphore::tryWait() {
    return sem_trywait(myHandle);
}


char Console::getc() {
    return PROJECT_BASE_V1_1_SYSCALL_HPP::getc();
}

void Console::putc(char c) {
    PROJECT_BASE_V1_1_SYSCALL_HPP::putc(c);
}


PeriodicThread::PeriodicThread(time_t period) : period(period), flag(true) {}

void PeriodicThread::terminate() {
    flag = false;
}

void PeriodicThread::run () {
    while (flag) {
        periodicActivation();
        time_sleep(period);
    }
}
