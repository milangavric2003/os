
#include "../h/riscv.hpp"
#include "../lib/console.h"
#include "../h/tcb.hpp"
#include "../h/print.hpp"
#include "../h/syscall_c.hpp"
#include "../h/MemoryAllocator.hpp"


void Riscv::popSppSpie() {
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");//jer se sad iz prekidne rutine ne izlazi putem handleSupervisorTrap
}

void Riscv::handleSupervisorTrap(){
    //ovde uvek ulazimo pa treba da vidimo sta je uzrok ulaska u ovu funkciju
    uint64 scause = r_scause();
    //nama od interesa supervisor software interrupt jer on stize od tajmera
    //supervisor external interrupt npr dolazi od konzole itd.
    //dakle kodovi 1 i 9 i interrupt=1
    if (scause == 0x0000000000000009UL) {//ovde kada neko eksplicitno pozove yield() sinhrono
        //interrupt: NO, cause code: environment call from S-mode (racunamo da je uvek S-mode a ne U-mode)
        uint64 code;
        __asm__ volatile ("mv %[code], a0" : [code] "=r"(code)); //mozda treba ovde sa fp da se skida
        if (code == MEM_ALLOC_CODE || code == MEM_FREE_CODE){
            uint64 sepc = r_sepc() + 4;//sve instrukcije 4 bajta => zelimo na prvu sledecu instrukciju tako funkc. ecall
            //ima u dokumentaciji
            uint64 sstatus = r_sstatus();
            //TCB::timeSliceCounter = 0;
            if (code == MEM_ALLOC_CODE) {
                uint64 size;
                __asm__ volatile ("mv %[size], a1" : [size] "=r"(size));
                MemoryAllocator::mem_alloc(size * MEM_BLOCK_SIZE);
            } else {
                uint64* ptr;
                __asm__ volatile ("mv %[ptr], a1" : [ptr] "=r"(ptr));
                MemoryAllocator::mem_free(ptr);
            }

            __asm__ volatile ("sd a0, 10 * 8(fp)"); // a0 na stek, tamo gde je i sacuvano

            //TCB::dispatch();//racunamo da niti ovde izlaze kada se izvrsi dispatch (sve su na ovaj nacin sacuvane)
            //a sta cemo za novo-napravljene niti
            w_sstatus(sstatus);
            w_sepc(sepc);//sepc nove niti
        } else {
            uint64 sepc = r_sepc() + 4;//sve instrukcije 4 bajta => zelimo na prvu sledecu instrukciju tako funkc. ecall
            //ima u dokumentaciji
            uint64 sstatus = r_sstatus();
            TCB::timeSliceCounter = 0;

            TCB::dispatch();//racunamo da niti ovde izlaze kada se izvrsi dispatch (sve su na ovaj nacin sacuvane)
            //a sta cemo za novo-napravljene niti
            w_sstatus(sstatus);
            w_sepc(sepc);//sepc nove niti
        }
    }
    if (scause == 0x8000000000000001UL) {
        //interrupt: yes, cause code: supervisor software interrupt (timer)
        TCB::timeSliceCounter++;
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice()) {
            //sepc registar bitan kada se pozove sret, zato i njega treba sacuvati
            uint64 sepc = r_sepc();//sepc stare niti
            uint64 sstatus = r_sstatus();
            TCB::timeSliceCounter = 0;
            printString("uso u timer rutinu\n");
            TCB::dispatch();//racunamo da niti ovde izlaze kada se izvrsi dispatch (sve su na ovaj nacin sacuvane)
                            //a sta cemo za novo-napravljene niti
            w_sstatus(sstatus);
            w_sepc(sepc);//sepc nove niti
        }
        mc_sip(SIP_SSIP);//supervisor interrupt pending = sip; vise nije ovaj supe. software interrupt
    } else if (scause == 0x8000000000000009UL) {
        //interrupt: yes, cause code: supervisor external interrupt (console)
        console_handler();//vec je implementirano
    } else {
        //unexptected trap cause - da vidimo sta je uzrok, gde se desilo i stval - trap value - dodatno opisuje interr.
        //print(scause)
        //print(sepc)
        //print(stval)
    }
}
