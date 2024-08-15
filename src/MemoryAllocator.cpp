#include "../h/MemoryAllocator.hpp"

BlockHeader* MemoryAllocator::allocatedMemHead = nullptr;

bool MemoryAllocator::instance = false;

BlockHeader* MemoryAllocator::freeMemHead = nullptr;

void* MemoryAllocator::mem_alloc (size_t size){

    if (size == 0) return nullptr;
    if (!instance) initialize();

    // Try to find an existing free block in the list (first fit):
    BlockHeader* blk = freeMemHead;
    BlockHeader* prev = nullptr;
    for (; blk!=nullptr; prev=blk, blk = blk->next)
        if (blk->size >= size) break;

    // If not found:
    if (blk == nullptr) {
        return nullptr; // No free memory
    }

    // Allocate the requested block:
    size_t remainingSize = blk->size - size;
    if (remainingSize >= MEM_BLOCK_SIZE + Block_Header_Size) { // at least one block size
        // A fragment remains
        blk->size = size;
        size_t offset = Block_Header_Size + size;
        BlockHeader* newBlk = (BlockHeader*) ((uint8*)blk + offset);
        if (prev) prev->next = newBlk;
        else freeMemHead = newBlk;
        newBlk->next = blk->next;
        newBlk->size = remainingSize - Block_Header_Size;
    } else {
        // No remaining fragment, allocate the entire block
        if (prev) prev->next = blk->next;
        else freeMemHead = blk->next;
    }
    blk->next = nullptr;

    // Add the block to the allocated list
    blk->next = allocatedMemHead;
    allocatedMemHead = blk;

    return (uint8*)blk + Block_Header_Size;
}

void MemoryAllocator::initialize() {
    freeMemHead = (BlockHeader*)HEAP_START_ADDR;
    freeMemHead->next = nullptr;
    freeMemHead->size = (size_t)HEAP_END_ADDR - (size_t)HEAP_START_ADDR - Block_Header_Size; // in bytes
    instance = true;
}

// Helper: Try to join cur with the cur->next free segment:
int MemoryAllocator::tryToJoin (BlockHeader* cur) {
    if (!cur) return 0;
    if (cur->next && (uint8*)cur + Block_Header_Size + cur->size == (uint8*)(cur->next)) {
        // Remove the cur->next segment:
        cur->size += cur->next->size + Block_Header_Size;
        cur->next = cur->next->next;
        return 1;
    } else
        return 0;
}

int MemoryAllocator::mem_free(void* addr_void) {

    if (addr_void == nullptr) return -2; // Can't free memory from nullptr

    // Initialize variables addr and size:
    uint8* addr = (uint8*)addr_void - Block_Header_Size; // addr points on BlockHeader
    size_t size = ((BlockHeader*)(addr))->size; // size of free block without BlockHeader
    BlockHeader* blk = (BlockHeader*)addr;

    // Check if the block is in the allocated list
    BlockHeader* prev = nullptr;
    BlockHeader* cur = allocatedMemHead;
    while (cur != nullptr && cur != blk) {
        prev = cur;
        cur = cur->next;
    }

    if (cur == nullptr) return -1; // Not already allocated with mem_alloc

    // Remove the block from the allocated list
    if (prev != nullptr) {
        prev->next = cur->next;
    } else {
        allocatedMemHead = cur->next;
    }

    // Find the place where to insert the new free segment (just after cur).
    prev = nullptr;
    BlockHeader* newSeg = nullptr;

    if (!freeMemHead || addr < (uint8*)freeMemHead) {
        // Insert as the first
        cur = freeMemHead;
        freeMemHead = (BlockHeader*)addr;
        newSeg = freeMemHead;
        freeMemHead->size = size;
        freeMemHead->next = cur;
    } else {
        for (cur = freeMemHead; cur != nullptr && addr > (uint8*)cur; prev = cur, cur = cur->next);

        // Insert the new segment after prev:
        newSeg = (BlockHeader*)addr;
        newSeg->size = size;
        newSeg->next = cur;
        prev->next = newSeg;
    }

    // Try to merge with the previous and next segments:
    tryToJoin(newSeg);
    tryToJoin(prev);
    return 0;
}

/*
MemoryAllocator::~MemoryAllocator() {
    //delete allocatedAddresses; ne treba jer na kraju svakako sve mora biti prazno (jer oslobodis sa mem free
    //                           sve sto si sa mem alloc napravio
}*/



