/*
#include "../h/tcb.hpp"
#include "../h/print.hpp"
#include "../h/workers.hpp"
#include "../h/riscv.hpp"
*/
/*#include "../lib/console.h" //tekuci u src pa zato ../

void main(){
    //put c i get c u console.h
    __putc('0');
    __putc('s');
    __putc('1');
    __putc('\n');
    __putc('\n');
    while(1){
        char character = __getc();
        __putc(character  + 30);
    }//ispisace i ) na kraju jer enter + 30 = (
    //moze program direktno iz terminala, dole jezicak ----> tada odmah kad napises 3 izbaci Q; da ne budu ovi znaci baferisani od strane razvoj. okruzenja3
    //"make qemu" komanda u terminalu
    //ctrl+a c, pa quit; a moze i samo ctrl+a x

    //next komanda u GDB jezicku debagera - kao step over
    //komanda help u GDB
    //apropos register
    //help info register
    //info registers ispisuje sve registre
    //info registers pc
    //x /10i $pc ---->ispise sve nadolazece instrukcije (exemin) (kao da sam u memory view kopiro memoriju iz pc-a)
    //display /1i $pc --- MORA PRVO OVO
    //stepi ---> kao step samo po masinskim instrukcijama
    //nexti

}*/


/*
int main(){
    TCB * threads[5];

    threads[0] = TCB::createThread(nullptr);
    TCB::running = threads[0];

    threads[1] = TCB::createThread(workerBodyA);
    printString("threadA created\n");//u ovim funkcijama zabraniti prekide da ne bi na pola stao ispis
    threads[2] = TCB::createThread(workerBodyB);
    printString("threadB created\n");
    threads[3] = TCB::createThread(workerBodyC);
    printString("threadC created\n");
    threads[4] = TCB::createThread(workerBodyD);
    printString("threadD created\n");

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);//adresa prekidne rutine u stvec
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);//da prekidi u supervisor modu budu prihvaceni

    while (!(threads[1]->isFinished() && threads[2]->isFinished()
        && threads[3]->isFinished() && threads[4]->isFinished())){
        TCB::yield();
    }//treba u while jer kada main pozove yield ubacujemo ga u sch
    //onda moze da se desi da opet njega izaberemo i da nikad ne izvrsimo bodyA i B

    for (auto &thread : threads){
        delete thread;
    }
    printString("Finished\n");

    return 0;
}
*/


#include "../lib/hw.h"
#include "../lib/console.h"
#include "../h/MemoryAllocator.hpp"

void checkNullptr(void* p) {
    static int x = 0;
    if(p == nullptr) {
        __putc('?');
        __putc('0' + x);
    }
    x++;
}

void checkStatus(int status) {
    static int y = 0;
    if(status) {
        __putc('0' + y);
        __putc('?');
    }
    y++;
}

int main() {
    int n = 16;
    char** matrix = (char**)MemoryAllocator::mem_alloc(n*sizeof(char*));
    checkNullptr(matrix);
    for(int i = 0; i < n; i++) {
        matrix[i] = (char *) MemoryAllocator::mem_alloc(n * sizeof(char));
        checkNullptr(matrix[i]);
    }

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            matrix[i][j] = 'k';
        }
    }

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            __putc(matrix[i][j]);
            __putc(' ');
        }
        __putc('\n');
    }


    for(int i = 0; i < n; i++) {
        int status = MemoryAllocator::mem_free(matrix[i]);
        checkStatus(status);
    }
    int status = MemoryAllocator::mem_free(matrix);
    checkStatus(status);

    return 0;
}



