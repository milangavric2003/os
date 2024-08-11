
#include "../h/riscv.hpp"
#include "../lib/console.h"
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
    if (scause == 0x0000000000000009UL) {//ovde kada neko eksplicitno pozove yield() sinhrono || scause == 0x0000000000000008UL
        //interrupt: NO, cause code: environment call from S-mode (racunamo da je uvek S-mode a ne U-mode)
        uint64 code;
        __asm__ volatile ("mv %[code], a0" : [code] "=r"(code)); //mozda treba ovde sa fp da se skida

        uint64 sepc = r_sepc() + 4;//sve instrukcije 4 bajta => zelimo na prvu sledecu instrukciju tako funkc. ecall
        //ima u dokumentaciji
        uint64 sstatus = r_sstatus();
        //TCB::timeSliceCounter = 0;
        switch (code) {
            case MEM_ALLOC_CODE:
                uint64 size;
                __asm__ volatile ("mv %[size], a1" : [size] "=r"(size));
                MemoryAllocator::mem_alloc(size * MEM_BLOCK_SIZE);
                __asm__ volatile ("sd a0, 10 * 8(fp)"); // a0 na stek, tamo gde je i sacuvano
                break;
            case MEM_FREE_CODE:
                uint64 *ptr;
                __asm__ volatile ("mv %[ptr], a1" : [ptr] "=r"(ptr));
                MemoryAllocator::mem_free(ptr);
                __asm__ volatile ("sd a0, 10 * 8(fp)"); // a0 na stek, tamo gde je i sacuvano
                break;
            case THREAD_CREATE_CODE:
                TCB **handle;
                __asm__ volatile ("mv %[handle], a1" : [handle] "=r"(handle));
                void(*start_routine)(void*);
                __asm__ volatile ("mv %[start_routine], a2" : [start_routine] "=r"(start_routine));
                void *arg;
                __asm__ volatile ("mv %[arg], a3" : [arg] "=r"(arg));
                void *stack_space;
                __asm__ volatile ("mv %[stack_space], a4" : [stack_space] "=r"(stack_space));

                TCB::createThread(handle, start_routine, arg, stack_space);
                __asm__ volatile ("sd a0, 10 * 8(fp)"); // a0 na stek, tamo gde je i sacuvano
                break;
            case THREAD_EXIT_CODE:
                TCB::thread_exit();
                //NE ZNAM KAD JE GRESKA!!!
                __asm__ volatile ("sd a0, 10 * 8(fp)"); // a0 na stek, tamo gde je i sacuvano
                break;
            case THREAD_DISPATCH_CODE:
                TCB::timeSliceCounter = 0;

                TCB::dispatch();//racunamo da niti ovde izlaze kada se izvrsi dispatch (sve su na ovaj nacin sacuvane)
                //a sta cemo za novo-napravljene niti
                break;
            default:
                break;
        }
        w_sstatus(sstatus);
        w_sepc(sepc);//sepc nove niti
    } else if (scause == 0x8000000000000001UL) {
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
