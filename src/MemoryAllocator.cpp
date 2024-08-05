#include "../h/MemoryAllocator.hpp"

List<uint8> MemoryAllocator::allocatedAddresses;

MemoryAllocator* MemoryAllocator::instance = nullptr;

BlockHeader* MemoryAllocator::freeMemHead = nullptr;

void* MemoryAllocator::mem_alloc (size_t size){

    if (size == 0) return nullptr;
    if (instance == nullptr) instance = new MemoryAllocator();

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
        else { freeMemHead = blk->next;
            /*if (blk->next) {
                // freeMemHead is never nullptr
                freeMemHead = blk->next;
            } else {
                // No memory at all
                freeMemHead = (BlockHeader*) ((uint8*)blk + Block_Header_Size + blk->size);
                freeMemHead->next = nullptr;
                freeMemHead->size = 0;
            }*/
        }
    }
    blk->next = nullptr;

    // Added because only memory allocated with mem_alloc can be freed with mem_free:
    allocatedAddresses.addFirst((uint8*)blk + Block_Header_Size);

    return (uint8*)blk + Block_Header_Size;
}

MemoryAllocator::MemoryAllocator(){
    freeMemHead = (BlockHeader*)HEAP_START_ADDR;
    freeMemHead->next = nullptr;
    freeMemHead->size = (size_t)HEAP_END_ADDR - (size_t)HEAP_START_ADDR - Block_Header_Size; // in bytes
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

    // Was it allocate with mem_alloc?
    if (allocatedAddresses.remove((uint8*)addr_void) == nullptr)
        return -1; // Not firstly allocated with mem_alloc

    // Initialize variables addr and size:
    uint8* addr = (uint8*)addr_void - Block_Header_Size; // addr points on BlockHeader
    size_t size = ((BlockHeader*)(addr))->size; // size of free block without BlockHeader

    // Find the place where to insert the new free segment (just after cur).
    BlockHeader* cur = nullptr;
    BlockHeader* prev = nullptr;
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



