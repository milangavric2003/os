#ifndef PROJECT_BASE_V1_1_LIST_HPP
#define PROJECT_BASE_V1_1_LIST_HPP

#include "MemoryAllocator.hpp"

template<typename T>
class List {
        private:
        struct Elem{
            T *data;
            Elem *next;

            Elem(T *data, Elem *next) : data(data), next(next) {}

            void* operator new(size_t size) {
                return MemoryAllocator::mem_alloc(size);
            }

            void operator delete(void* ptr) {
                MemoryAllocator::mem_free(ptr);
            }

            void* operator new[](size_t size) {
                return MemoryAllocator::mem_alloc(size);
            }

            void operator delete[](void* ptr) {
                MemoryAllocator::mem_free(ptr);
            }
        };

        Elem *head, *tail;
        Elem *temp;

        public:
        List() : head(0), tail(0), temp(0) {}

        List(const List<T> &) = delete;

        List<T> &operator=(const List<T> &) = delete;
        /*
        ~List(){
            while (head) {
                printString("nije sve bilo izbaceno iz liste\n");
                Elem *next = head->next;
                delete head;
                head = next;
            }
            tail = nullptr;
        }*/

        void addFirst(T *data){
            Elem *elem = new Elem(data, head);
            head = elem;
            if (!tail) tail = head;
        }

        void addLast(T *data){
            Elem *elem = new Elem(data, 0);
            if (tail){
                tail -> next = elem;
                tail = elem;
            } else {
                head = tail = elem;
            }
        }

        T *removeFirst(){
            if (!head) return 0;

            Elem *elem = head;
            head = head->next;
            if (!head) tail = 0;

            T *ret = elem->data;
            delete elem;
            return ret;
        }

        T *peekFirst(){
            if (!head) return 0;
            return head->data;
        }

        T *removeLast(){
            if (!head) return 0;
            Elem *prev = 0;
            for (Elem * curr = head; curr && curr != tail; curr = curr->next){
                prev = curr;
            }

            Elem *elem = tail;
            if (prev) prev->next = 0;
            else head = 0;
            tail = prev;

            T *ret = elem->data;
            delete elem;
            return ret;
        }

        T *peekLast(){
            if (!tail) return 0;
            return tail->data;
        }

        T* remove (T* toBeRemoved){
            if (!head) return nullptr; // List is empty

            Elem *prev = nullptr;
            Elem *curr = head;

            while (curr != nullptr) {
                if (curr->data == toBeRemoved) {
                    // Element found, remove it
                    if (prev) {
                        prev->next = curr->next;
                    } else {
                        head = curr->next; // Removing the head element
                    }

                    if (curr == tail) {
                        tail = prev; // Update tail if necessary
                    }

                    T *ret = curr->data;
                    delete curr;
                    return ret;
                }

                prev = curr;
                curr = curr->next;
            }

            return nullptr; // Element not found
        }

        T* peekNext() {
            if (temp == nullptr) temp = head;
            if (temp && temp->next) {
                temp = temp->next;
                return temp->data;
            }
            temp = nullptr;
            return nullptr;
        }

        void resetTemp() {
            temp = nullptr;
        }

        void addBefore(T* toAdd, T* beforeThis) {
            if (!head) return; // List is empty

            Elem *elem = new Elem(toAdd, nullptr);
            Elem *prev = nullptr;
            Elem *curr = head;

            while (curr != nullptr) {
                if (curr->data == beforeThis) {
                    // Element found, add before it
                    elem->next = curr;
                    if (!prev) {
                        head = elem;
                    } else {
                        prev->next = elem;
                    }
                    return;
                }
                prev = curr;
                curr = curr->next;
            }

            delete elem;
        }

};

#endif