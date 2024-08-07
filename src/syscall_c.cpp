#include "../h/syscall_c.hpp"

void* mem_alloc (size_t size){
    size = (size + MEM_BLOCK_SIZE - 1 ) / MEM_BLOCK_SIZE;

    __asm__ volatile ("mv a1, %[size]" : : [size] "r"(size)); // a1 <= size
    __asm__ volatile ("mv a0, %[MEM_ALLOC_CODE]" : : [MEM_ALLOC_CODE] "r"(MEM_ALLOC_CODE)); // a0 <= code

    __asm__ volatile ("ecall"); // ovo vodi u prekidnu rutinu

    void* result;
    __asm__ volatile ("mv %[result], a0" : [result] "=r"(result)); // result <= a0

    return result;
}

int mem_free(void* ptr){
    __asm__ volatile ("mv a1, %[ptr]" : : [ptr] "r"(ptr)); // a1 <= ptr
    __asm__ volatile ("mv a0, %[MEM_FREE_CODE]" : : [MEM_FREE_CODE] "r"(MEM_FREE_CODE)); // a0 <= code

    __asm__ volatile ("ecall"); // ovo vodi u prekidnu rutinu

    int result;
    __asm__ volatile ("mv %[result], a0" : [result] "=r"(result)); // result <= a0

    return result;
}

