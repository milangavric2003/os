#ifndef PROJECT_BASE_V1_1_RISCV_HPP
#define PROJECT_BASE_V1_1_RISCV_HPP

#include "../lib/hw.h"

class Riscv
{
public:

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
    static void wstval(uint64 stval);

    enum BitMaskSip
    {
            SIP_SSIE = (1 << 1),
            SIP_STIE = (1 << 5),
            SIP_SEIE = (1 << 9),
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

private:

};

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

#endif