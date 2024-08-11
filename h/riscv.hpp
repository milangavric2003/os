#ifndef PROJECT_BASE_V1_1_RISCV_HPP
#define PROJECT_BASE_V1_1_RISCV_HPP

#include "../lib/hw.h"
#include "../h/syscall_c.hpp"

class Riscv
{
public:

    //exit running thread
    static int thread_exit();

    //create thread
    static int thread_create (thread_t* handle, void(*start_routine)(void*),
                              void* arg, void* stack_space);

    //pop sstatus.spp i sstatus.spie bits (has to be a non inline function)
    static void popSppSpie();

    //push x3...x31 registers on stack
    static void pushRegisters();

    //pop x3...x31 registers on stack
    static void popRegisters();

    // read register scouse
    static uint64 r_scause();

    // write register scouse
    static void w_scause (uint64 scause);

    // read register sepc
    static uint64 r_sepc();

    // write register sepc
    static void w_sepc(uint64 sepc);

    // read register stvec
    static uint64 r_stvec();

    // write register stvec
    static void w_stvec(uint64 stvec);

    // read register stval
    static uint64 r_stval();

    // write register stval
    static void w_stval(uint64 stval);

    enum BitMaskSip
    {
            SIP_SSIP = (1 << 1),
            SIP_STIP = (1 << 5),
            SIP_SEIP = (1 << 9),
    };

    // mask set register sip
    static void ms_sip(uint64 mask);

    // mask cleur register sip
    static void mc_sip(uint64 mask);

    // read register sip
    static uint64 r_sip();

    // write register sip
    static void w_sip(uint64 sip);

    enum BitMaskSstatus
    {
        SSTATUS_SIE = (1 << 1),
        SSTATUS_SPIE = (1 << 5),
        SSTATUS_SPP = (1 << 8),
    };

    // mask set register sstatus
    static void ms_sstatus(uint64 mask);

    // mask clear register sstatus
    static void mc_sstatus(uint64 mask);

    // read register sstatus
    static uint64 r_sstatus();

    // write register sstatus
    static void w_sstatus(uint64 sstatus);

    //supervisor trap - ovo poziva kada se javi exception, ali procesoru treba reci da ovo treba da zove
    static void supervisorTrap();

private:

    //supervisor trap handler
    static void handleSupervisorTrap();

};

inline int Riscv::thread_exit (){
    __asm__ volatile ("mv a0, %[THREAD_EXIT_CODE]" : : [THREAD_EXIT_CODE] "r"(THREAD_EXIT_CODE));
    // a0 <= THREAD_EXIT_CODE

    __asm__ volatile ("ecall"); // ovo vodi u prekidnu rutinu

    int result;
    __asm__ volatile ("mv %[result], a0" : [result] "=r"(result)); // result <= a0

    return result;
}

inline int Riscv::thread_create (thread_t* handle, void(*start_routine)(void*),
                          void* arg, void* stack_space){
    __asm__ volatile ("mv a1, %[handle]" : : [handle] "r"(handle)); // a1 <= handle
    __asm__ volatile ("mv a2, %[start_routine]" : : [start_routine] "r"(start_routine)); // a2 <= start_routine
    __asm__ volatile ("mv a3, %[arg]" : : [arg] "r"(arg)); // a3 <= arg
    __asm__ volatile ("mv a4, %[stack_space]" : : [stack_space] "r"(stack_space)); // a4 <= stack_space
    __asm__ volatile ("mv a0, %[THREAD_CREATE_CODE]" : : [THREAD_CREATE_CODE] "r"(THREAD_CREATE_CODE));
    // a0 <= THREAD_CREATE_CODE

    __asm__ volatile ("ecall"); // ovo vodi u prekidnu rutinu

    int result;
    __asm__ volatile ("mv %[result], a0" : [result] "=r"(result)); // result <= a0

    return result;
}

inline uint64 Riscv::r_scause()
{
    uint64 volatile scause;
    __asm__ volatile ("csrr %[scause], scause" : [scause] "=r" (scause));
    return scause;
}

inline void Riscv::w_scause (uint64 scause)
{
    __asm__ volatile ("csrw scause, %[scause]" : : [scause] "r"(scause));
}

inline uint64 Riscv::r_sepc(){
    uint64 volatile sepc;
    __asm__ volatile ("csrr %[sepc], sepc" : [sepc] "=r"(sepc));
    return sepc;
}

inline void Riscv::w_sepc(uint64 sepc) {
    __asm__ volatile ("csrw sepc, %[sepc]" : : [sepc] "r"(sepc));
}

inline uint64 Riscv::r_stvec() {
    uint64 volatile stvec;
    __asm__ volatile ("csrr %[stvec], stvec" : [stvec] "=r"(stvec));
    return stvec;
}

inline void Riscv::w_stvec(uint64 stvec) {
    __asm__ volatile ("csrw stvec, %[stvec]" : : [stvec] "r"(stvec));
}

inline uint64 Riscv::r_stval() {
    uint64 volatile stval;
    __asm__ volatile ("csrr %[stval], stval" : [stval] "=r"(stval));
    return stval;
}

inline void Riscv::w_stval(uint64 stval) {
    __asm__ volatile ("csrw stval, %[stval]" : : [stval] "r"(stval));
}

inline void Riscv::ms_sip(uint64 mask) {
    __asm__ volatile ("csrs sip, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::mc_sip(uint64 mask) {
    __asm__ volatile ("csrc sip, %[mask]" : : [mask] "r"(mask));
}

inline uint64 Riscv::r_sip() {
    uint64 volatile sip;
    __asm__ volatile ("csrr %[sip], sip" : [sip] "=r"(sip));
    return sip;
}

inline void Riscv::w_sip(uint64 sip) {
    __asm__ volatile ("csrw sip, %[sip]" : : [sip] "r"(sip));
}

inline void Riscv::ms_sstatus(uint64 mask) {
    __asm__ volatile ("csrs sstatus, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::mc_sstatus(uint64 mask) {
    __asm__ volatile ("csrc sstatus, %[mask]" : : [mask] "r"(mask));
}

inline uint64 Riscv::r_sstatus() {
    uint64 volatile sstatus;
    __asm__ volatile ("csrr %[sstatus], sstatus" : [sstatus] "=r"(sstatus));
    return sstatus;
}

inline void Riscv::w_sstatus(uint64 sstatus) {
    __asm__ volatile ("csrw sstatus, %[sstatus]" : : [sstatus] "r"(sstatus));
}

#endif