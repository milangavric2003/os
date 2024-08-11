

#include "../h/print.hpp"
#include "../h/syscall_c.hpp"
#include "../h/riscv.hpp"
#include "../lib/console.h"
#include "../h/workers.hpp"


int main(){

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);//adresa prekidne rutine u stvec
    void* arg = nullptr;
    thread_t* handlemain = new (thread_t);
    thread_create(handlemain, nullptr, arg);
    TCB::running = *handlemain;

    thread_t* handleA = new (thread_t);
    thread_t* handleB = new (thread_t);
    thread_t* handleC = new (thread_t);
    thread_t* handleD = new (thread_t);
    thread_t* threads[5] = {handlemain, handleA, handleB, handleC, handleD};

    thread_create(handleA, workerBodyA, arg);
    printString("threadA created\n");//u ovim funkcijama zabraniti prekide da ne bi na pola stao ispis
    thread_create(handleB, workerBodyB, arg);
    printString("threadB created\n");
    thread_create(handleC, workerBodyC, arg);
    printString("threadC created\n");
    thread_create(handleD, workerBodyD, arg);
    printString("threadD created\n");

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);//da prekidi u supervisor modu budu prihvaceni

    while (!((*threads[1])->isFinished() && (*threads[2])->isFinished()
             && (*threads[3])->isFinished() && (*threads[4])->isFinished())){
        TCB::yield();
    }//treba u while jer kada main pozove yield ubacujemo ga u sch
    //onda moze da se desi da opet njega izaberemo i da nikad ne izvrsimo bodyA i B

    for (auto &thread : threads){
        delete *thread;
    }
    printString("Finished\n");

    return 0;
}



