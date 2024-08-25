#include "../h/syscall_c.hpp"
#include "../h/riscv.hpp"
#include "../test/printing.hpp"

void userMain();

int main () {

    TCB *threads[5];

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    //Riscv::ms_sstatus(Riscv::SSTATUS_SIE);
    thread_create(&threads[0],nullptr,nullptr);
    TCB::running = threads[0];

    thread_create(&threads[1], reinterpret_cast<void (*)(void *)>(userMain), nullptr);

    while(!threads[1]->isFinished()) {
        thread_dispatch();
    }

    return 0;

}
