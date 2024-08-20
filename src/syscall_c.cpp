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
    if (handle == nullptr) return -2; // handle is nullptr
    void* stack_space = mem_alloc(DEFAULT_STACK_SIZE);
    if (!stack_space) return -1; // mem_alloc error
    return Riscv::thread_create(handle, start_routine, arg, stack_space);
}

int thread_exit(){
    int status = Riscv::thread_exit();
    if (status == 0)
        TCB::yield(); //valjda nmp ovo treba da se pozove nmp
    return status;
}

void thread_dispatch(){
    TCB::yield();
}

int sem_open (sem_t* handle, unsigned init) {
    if (handle == nullptr) return -2; // handle is nullptr
    __asm__ volatile ("mv a2, %[init]" : : [init] "r"(init)); // a2 <= init
    __asm__ volatile ("mv a1, %[handle]" : : [handle] "r"(handle)); // a1 <= handle
    __asm__ volatile ("mv a0, %[SEM_OPEN_CODE]" : : [SEM_OPEN_CODE] "r"(SEM_OPEN_CODE)); // a0 <= SEM_OPEN_CODE

    __asm__ volatile ("ecall"); // ovo vodi u prekidnu rutinu

    int result;
    __asm__ volatile ("mv %[result], a0" : [result] "=r"(result)); // result <= a0

    return result;
}

int sem_close (sem_t id) {
    if (id == nullptr) return -2; // handle is nullptr
    __asm__ volatile ("mv a1, %[id]" : : [id] "r"(id)); // a1 <= id
    __asm__ volatile ("mv a0, %[SEM_CLOSE_CODE]" : : [SEM_CLOSE_CODE] "r"(SEM_CLOSE_CODE)); // a0 <= SEM_CLOSE_CODE

    __asm__ volatile ("ecall"); // ovo vodi u prekidnu rutinu

    int result;
    __asm__ volatile ("mv %[result], a0" : [result] "=r"(result)); // result <= a0

    return result;
}

int sem_wait (sem_t id) {
    if (id == nullptr) return -2; // handle is nullptr
    __asm__ volatile ("mv a1, %[id]" : : [id] "r"(id)); // a1 <= id
    __asm__ volatile ("mv a0, %[SEM_WAIT_CODE]" : : [SEM_WAIT_CODE] "r"(SEM_WAIT_CODE)); // a0 <= SEM_WAIT_CODE

    __asm__ volatile ("ecall"); // ovo vodi u prekidnu rutinu

    int result;
    __asm__ volatile ("mv %[result], a0" : [result] "=r"(result)); // result <= a0

    return result;
}

int sem_signal (sem_t id) {
    if (id == nullptr) return -2; // handle is nullptr
    __asm__ volatile ("mv a1, %[id]" : : [id] "r"(id)); // a1 <= id
    __asm__ volatile ("mv a0, %[SEM_SIGNAL_CODE]" : : [SEM_SIGNAL_CODE] "r"(SEM_SIGNAL_CODE)); // a0 <= SEM_SIGNAL_CODE

    __asm__ volatile ("ecall"); // ovo vodi u prekidnu rutinu

    int result;
    __asm__ volatile ("mv %[result], a0" : [result] "=r"(result)); // result <= a0

    return result;
}

int sem_timedwait (sem_t id, time_t timeout) {
    return 0;
}

int sem_trywait (sem_t id) {
    if (id == nullptr) return -2; // handle is nullptr
    __asm__ volatile ("mv a1, %[id]" : : [id] "r"(id)); // a1 <= id
    __asm__ volatile ("mv a0, %[SEM_TRYWAIT_CODE]" : : [SEM_TRYWAIT_CODE] "r"(SEM_TRYWAIT_CODE)); // a0 <= SEM_TRYWAIT_CODE

    __asm__ volatile ("ecall"); // ovo vodi u prekidnu rutinu

    int result;
    __asm__ volatile ("mv %[result], a0" : [result] "=r"(result)); // result <= a0

    return result;
}

int time_sleep (time_t) {
    return 0;
}

char getc () {
    __asm__ volatile ("mv a0, %[GETC_CODE]" : : [GETC_CODE] "r"(GETC_CODE)); // a0 <= GETC_CODE

    __asm__ volatile ("ecall"); // ovo vodi u prekidnu rutinu

    char result;
    __asm__ volatile ("mv %[result], a0" : [result] "=r"(result)); // result <= a0

    return result;
}

void putc (char c) {
    __asm__ volatile ("mv a1, %[c]" : : [c] "r"(c)); // a1 <= c
    __asm__ volatile ("mv a0, %[PUTC_CODE]" : : [PUTC_CODE] "r"(PUTC_CODE)); // a0 <= PUTC_CODE

    __asm__ volatile ("ecall"); // ovo vodi u prekidnu rutinu

}
