#include "../h/BufferPomocni.hpp"

BufferPomocni::BufferPomocni(int _cap) : cap(_cap + 1), head(0), tail(0) {
    buffer = (char *)MemoryAllocator::mem_alloc(1 * cap);
    itemAvailable = new SemaphorePomocni(0); // moze greska sa alokacijom
    spaceAvailable = new SemaphorePomocni(_cap);
}

BufferPomocni::~BufferPomocni() {
    /*putc('\n');
    printString("BufferPomocni deleted!\n");
    while (getCnt() > 0) {
        char ch = buffer[head];
        putc(ch);
        head = (head + 1) % cap;
    }
    putc('!');
    putc('\n');*/

    MemoryAllocator::mem_free(buffer);
    itemAvailable->close();
    spaceAvailable->close();
    delete itemAvailable;
    delete spaceAvailable;

}

void BufferPomocni::put(char val) {
    spaceAvailable->wait();

    buffer[tail] = val;
    tail = (tail + 1) % cap;

    itemAvailable->signal();

}

char BufferPomocni::get() {
    itemAvailable->wait();

    char ret = buffer[head];
    head = (head + 1) % cap;

    spaceAvailable->signal();

    return ret;
}

int BufferPomocni::getCnt() {
    int ret;

    if (tail >= head) {
        ret = tail - head;
    } else {
        ret = cap - head + tail;
    }

    return ret;
}
