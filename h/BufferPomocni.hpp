#ifndef PROJECT_BASE_V1_1_BUFFERPOMOCNI_HPP
#define PROJECT_BASE_V1_1_BUFFERPOMOCNI_HPP

#include "syscall_c.hpp"

class BufferPomocni {
private:
    int cap;
    char *buffer;
    int head, tail;

    sem_t spaceAvailable;
    sem_t itemAvailable;

public:
    BufferPomocni(int _cap = 256);
    ~BufferPomocni();

    void put(char val);
    char get();

    int getCnt();

    /*void* operator new(size_t size) {
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
    }*/

};

#endif