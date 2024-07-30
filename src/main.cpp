
#include "../h/tcb.hpp"
#include "../h/print.hpp"
#include "../h/workers.hpp"
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


int main(){
    TCB * coroutines[3];

    coroutines[0] = TCB::createCoroutine(nullptr);
    TCB::running = coroutines[0];

    coroutines[1] = TCB::createCoroutine(workerBodyA);
    printString("CoroutineA created\n");
    coroutines[2] = TCB::createCoroutine(workerBodyB);
    printString("CoroutineB created\n");

    while (!(coroutines[1]->isFinished() && coroutines[2]->isFinished())){
        TCB::yield();
    }//treba u while jer kada main pozove yield ubacujemo ga u sch
    //onda moze da se desi da opet njega izaberemo i da nikad ne izvrsimo bodyA i B

    for (auto &coroutine : coroutines){
        delete coroutine;
    }
    printString("Finished\n");

    return 0;
}

