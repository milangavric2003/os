#include "../h/syscall_c.hpp"
#include "../h/riscv.hpp"

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

int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg){
    void* stack_space = mem_alloc(DEFAULT_STACK_SIZE);
    if (!stack_space) return -1; // mem_alloc error
    return Riscv::thread_create(handle, start_routine, arg, stack_space);
}

extern int thread_exit(){
    int status = Riscv::thread_exit();
    if (status == 0)
        TCB::yield(); //valjda nmp ovo treba da se pozove nmp
    return status;
}

extern void thread_dispatch(){
    TCB::yield();
}




