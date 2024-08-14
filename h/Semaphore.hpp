#ifndef PROJECT_BASE_V1_1_SEMAPHORE_HPP
#define PROJECT_BASE_V1_1_SEMAPHORE_HPP

#include "tcb.hpp"

class Semaphore {
public :
    Semaphore (unsigned short init = 1) : val(init), ret(0) {}
    ~Semaphore();

    int wait ();
    void signal ();
    int value () const { return val; }

protected:
    void block ();
    void unblock ();

private :
    int val;
    List<TCB> blocked;
    int ret;
} ;

#endif