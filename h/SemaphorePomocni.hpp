#ifndef PROJECT_BASE_V1_1_SEMAPHOREPOMOCNI_HPP
#define PROJECT_BASE_V1_1_SEMAPHOREPOMOCNI_HPP

#include "tcb.hpp"

class SemaphorePomocni {
public :
    SemaphorePomocni (unsigned short init = 1) : val(init), ret(0) {}
    //~SemaphorePomocni();

    int wait ();
    int signal ();
    int trywait();
    int timedwait(time_t);
    //int value () const { return val; }
    int close();

    void* operator new(size_t size) {
        return MemoryAllocator::mem_alloc(size);
    }

    void operator delete(void* ptr) {
        MemoryAllocator::mem_free(ptr);
    }

    void* operator new[](size_t size) {
        return MemoryAllocator::mem_alloc(size);
    }

    void operator delete[](void* ptr) {
        MemoryAllocator::mem_free(ptr);
    }

protected:
    void block ();
    void unblock ();

private :
    int val;
    List<TCB> blocked;
    int ret;
    static int constexpr SEMDEAD = -1;
    static int constexpr TIMEOUT = -2;
    static int constexpr SIGNAL_ERROR = -3;
} ;

#endif