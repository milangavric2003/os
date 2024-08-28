#include "../h/riscv.hpp"
#include "../lib/console.h"
#include "../test/printing.hpp"

BufferPomocni* Riscv::inputBuffer = nullptr;
BufferPomocni* Riscv::outputBuffer = nullptr;

void Riscv::outThrBody(void* arg) {
    while (true) {
        while (CONSOLE_TX_STATUS_BIT & *((char*)CONSOLE_STATUS)) {
            *(char*)CONSOLE_TX_DATA = outputBuffer->get();
        }
    }
}

void Riscv::popSppSpie() {
    if(TCB::running->body == nullptr || TCB::running->body == &outThrBody) {
        Riscv::ms_sstatus(Riscv::SSTATUS_SPP);
    }
    else {
        Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
    }
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");//jer se sad iz prekidne rutine ne izlazi putem handleSupervisorTrap
}

void Riscv::handleSupervisorTrap(){
    //ovde uvek ulazimo pa treba da vidimo sta je uzrok ulaska u ovu funkciju
    uint64 scause = r_scause();
    //nama od interesa supervisor software interrupt jer on stize od tajmera
    //supervisor external interrupt npr dolazi od konzole itd.
    //dakle kodovi 1 i 9 i interrupt=1
    if (scause == 0x0000000000000009UL || scause == 0x0000000000000008UL) {//ovde kada neko eksplicitno pozove yield() sinhrono || scause == 0x0000000000000008UL
        //interrupt: NO, cause code: environment call from S-mode (racunamo da je uvek S-mode a ne U-mode)
        //or U-mode
        uint64 code;
        __asm__ volatile ("ld %[code], 10 * 8(fp)" : [code] "=r"(code)); //mozda treba ovde sa fp da se skida

        uint64 volatile sepc = r_sepc() + 4;//sve instrukcije 4 bajta => zelimo na prvu sledecu instrukciju tako funkc. ecall
        //ima u dokumentaciji
        uint64 volatile sstatus = r_sstatus();
        //TCB::timeSliceCounter = 0;
        switch (code) {
            case MEM_ALLOC_CODE:
                uint64 size;
                __asm__ volatile ("ld %[size], 11 * 8(fp)" : [size] "=r"(size));
                MemoryAllocator::mem_alloc(size * MEM_BLOCK_SIZE);
                __asm__ volatile ("sd a0, 10 * 8(fp)"); // a0 na stek, tamo gde je i sacuvano
                break;
            case MEM_FREE_CODE:
                uint64 *ptr;
                __asm__ volatile ("ld %[ptr], 11 * 8(fp)" : [ptr] "=r"(ptr));
                MemoryAllocator::mem_free(ptr);
                __asm__ volatile ("sd a0, 10 * 8(fp)"); // a0 na stek, tamo gde je i sacuvano
                break;
            case THREAD_CREATE_CODE:
                TCB **handle;
                __asm__ volatile ("ld %[handle], 11 * 8(fp)" : [handle] "=r"(handle));
                void(*start_routine)(void*);
                __asm__ volatile ("ld %[start_routine], 12 * 8(fp)" : [start_routine] "=r"(start_routine));
                void *arg;
                __asm__ volatile ("ld %[arg], 13 * 8(fp)" : [arg] "=r"(arg));
                void *stack_space;
                __asm__ volatile ("ld %[stack_space], 14 * 8(fp)" : [stack_space] "=r"(stack_space));

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
            case SEM_OPEN_CODE:
                sem_t *id;
                __asm__ volatile ("ld %[id], 11 * 8(fp)" : [id] "=r"(id));
                unsigned init;
                __asm__ volatile ("ld %[init], 12 * 8(fp)" : [init] "=r"(init));

                if ((*id = new SemaphorePomocni(init)) == nullptr) { //nece moci ovako-new!!!!!!!!!!
                    __asm__ volatile ("li a0, -1"); // greska sa alokacijom
                } else {
                    __asm__ volatile ("li a0, 0");
                }
                __asm__ volatile ("sd a0, 10 * 8(fp)"); // a0 na stek, tamo gde je i sacuvano
                break;
            case SEM_CLOSE_CODE:
                sem_t idClose;
                __asm__ volatile ("ld %[idClose], 11 * 8(fp)" : [idClose] "=r"(idClose));

                /*if ((delete idClose)) { // GRESKA SA DEALOKACIJOM (kad budes menjo new i delete) (moze jos greska ukoliko handle nije postojeci
                    __asm__ volatile ("li a0, -1"); // greska sa alokacijom
                } else {
                    __asm__ volatile ("li a0, 0");
                }*/
                idClose->close();
                __asm__ volatile ("sd a0, 10 * 8(fp)"); // a0 na stek, tamo gde je i sacuvano
                break;
            case SEM_WAIT_CODE:
                sem_t idWait;
                __asm__ volatile ("ld %[idWait], 11 * 8(fp)" : [idWait] "=r"(idWait));

                idWait->wait();
                __asm__ volatile ("sd a0, 10 * 8(fp)"); // a0 na stek, tamo gde je i sacuvano
                break;
            case SEM_SIGNAL_CODE:
                sem_t idSignal;
                __asm__ volatile ("ld %[idSignal], 11 * 8(fp)" : [idSignal] "=r"(idSignal));

                idSignal->signal();
                __asm__ volatile ("sd a0, 10 * 8(fp)"); // a0 na stek, tamo gde je i sacuvano
                break;
            case SEM_TRYWAIT_CODE:
                sem_t idTrywait;
                __asm__ volatile ("ld %[idTrywait], 11 * 8(fp)" : [idTrywait] "=r"(idTrywait));

                idTrywait->trywait();
                __asm__ volatile ("sd a0, 10 * 8(fp)"); // a0 na stek, tamo gde je i sacuvano
                break;
            case SEM_TIMEDWAIT_CODE:
                sem_t idTimedwait;
                __asm__ volatile ("ld %[idTimedwait], 11 * 8(fp)" : [idTimedwait] "=r"(idTimedwait));
                time_t timeout;
                __asm__ volatile ("ld %[timeout], 12 * 8(fp)" : [timeout] "=r"(timeout));

                idTimedwait->timedwait(timeout);
                __asm__ volatile ("sd a0, 10 * 8(fp)"); // a0 na stek, tamo gde je i sacuvano
                break;
            case GETC_CODE:

                inputBuffer->get();
                //__getc();
                __asm__ volatile ("sd a0, 10 * 8(fp)"); // a0 na stek, tamo gde je i sacuvano
                break;
            case PUTC_CODE:
                char c;
                __asm__ volatile ("ld %[c], 11 * 8(fp)" : [c] "=r"(c));

                outputBuffer->put(c);
                //__putc(c);
                break;
            case TIME_SLEEP_CODE:
                time_t time;
                __asm__ volatile ("ld %[time], 11 * 8(fp)" : [time] "=r"(time));

                TCB::time_sleep(time);
                __asm__ volatile ("sd a0, 10 * 8(fp)"); // a0 na stek, tamo gde je i sacuvano
                break;
            default:
                break;
        }
        w_sstatus(sstatus);
        w_sepc(sepc);//sepc nove niti
    } else if (scause == 0x8000000000000001UL) {
        //interrupt: yes, cause code: supervisor software interrupt (timer)

        TCB::timer_tick();

        mc_sip(SIP_SSIP);//supervisor interrupt pending = sip; vise nije ovaj supe. software interrupt
        TCB::timeSliceCounter++;
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice()) {
            //sepc registar bitan kada se pozove sret, zato i njega treba sacuvati
            uint64 volatile sepc = r_sepc();//sepc stare niti
            uint64 volatile sstatus = r_sstatus();
            TCB::timeSliceCounter = 0;
            TCB::dispatch();//racunamo da niti ovde izlaze kada se izvrsi dispatch (sve su na ovaj nacin sacuvane)
                            //a sta cemo za novo-napravljene niti
            w_sstatus(sstatus);
            w_sepc(sepc);//sepc nove niti
        }
    } else if (scause == 0x8000000000000009UL) {
        //interrupt: yes, cause code: supervisor external interrupt (console)
        int irq = plic_claim();

        while (CONSOLE_RX_STATUS_BIT & *((char*)CONSOLE_STATUS)) {
            inputBuffer->put(*(char*)CONSOLE_RX_DATA);
        }
        //console_handler();//vec je implementirano

        plic_complete(irq);
    } else {
        //unexptected trap cause - da vidimo sta je uzrok, gde se desilo i stval - trap value - dodatno opisuje interr.
        //Riscv::ms_sstatus(Riscv::SSTATUS_SIE);
        while (true) {
            printString("ERROR!!! scause: ");
            printInt(scause);
            printString("\n");
        }
        //while(true)TCB::dispatch();
        //Riscv::mc_sstatus(Riscv::SSTATUS_SIE);
    }
}
