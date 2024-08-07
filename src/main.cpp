

#include "../h/print.hpp"
#include "../h/syscall_c.hpp"
#include "../h/riscv.hpp"
#include "../lib/console.h"



int main() {
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);//adresa prekidne rutine u stvec
    int n = 10;
    char* niz = (char*)mem_alloc(10*sizeof(char));
    if(niz == nullptr) {
        __putc('?');
    }

    for(int i = 0; i < n; i++) {
        niz[i] = 'k';
    }

    for(int i = 0; i < n; i++) {
        __putc(niz[i]);
        __putc(' ');
    }

    int status = mem_free(niz);
    if(status != 0) {
       // __putc('?');
    }

    return 0;

}



