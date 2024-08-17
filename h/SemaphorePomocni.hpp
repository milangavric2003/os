#ifndef PROJECT_BASE_V1_1_SEMAPHOREPOMOCNI_HPP
#define PROJECT_BASE_V1_1_SEMAPHOREPOMOCNI_HPP

#include "tcb.hpp"

class SemaphorePomocni {
public :
    SemaphorePomocni (unsigned short init = 1) : val(init), ret(0) {}
    ~SemaphorePomocni();

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