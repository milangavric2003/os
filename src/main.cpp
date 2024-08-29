#include "../h/syscall_c.hpp"
#include "../h/riscv.hpp"
#include "../test/printing.hpp"

void userMain();

int main () {

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    Riscv::inputBuffer = new BufferPomocni;
    Riscv::outputBuffer = new BufferPomocni;

    TCB *threads[5];


    thread_create(&threads[0],nullptr,nullptr);
    TCB::running = threads[0];

    // output thread for console
    thread_create(&threads[2], Riscv::outThrBody, nullptr);

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    thread_create(&threads[1], reinterpret_cast<void (*)(void *)>(userMain), nullptr);

    while(!threads[1]->isFinished() || Riscv::outputBuffer->getCnt() > 0) {
        thread_dispatch();
    }

    Riscv::mc_sstatus(Riscv::SSTATUS_SIE);

    delete Riscv::inputBuffer;
    delete Riscv::outputBuffer;

    return 0;

}

