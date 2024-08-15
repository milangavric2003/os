#include "../lib/hw.h"
#include "../lib/console.h"
#include "../h/syscall_c.hpp"
#include "../h/riscv.hpp"

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
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);//adresa prekidne rutine u stvec
    int n = 16;
    char** matrix = (char**)mem_alloc(n*sizeof(char*));
    checkNullptr(matrix);
    for(int i = 0; i < n; i++) {
        matrix[i] = (char *) mem_alloc(n * sizeof(char));
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
        int status = mem_free(matrix[i]);
        checkStatus(status);
    }
    int status = mem_free(matrix);
    checkStatus(status);

    return 0;
}