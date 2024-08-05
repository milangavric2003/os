#ifndef PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"
#include "list.hpp"

struct BlockHeader{
    BlockHeader *next;
    size_t size; // in bytes
};

class MemoryAllocator
{
private:

    static List<uint8> allocatedAddresses; // list of pointers on uint8 (on bytes)

    static BlockHeader *freeMemHead;

    static MemoryAllocator* instance;

    static size_t constexpr Block_Header_Size = 16; // 16 bytes

    MemoryAllocator(); // no public constructor

    //~MemoryAllocator(); // no public constructor

    static int tryToJoin (BlockHeader* cur);

public:

    static void destruct() {
        delete instance;
        instance = nullptr;
    }

    MemoryAllocator(const MemoryAllocator &) = delete;

    MemoryAllocator &operator=(const MemoryAllocator &) = delete;

    static void* mem_alloc (size_t size); //size is in bytes

    static int mem_free (void* addr);

};

#endif