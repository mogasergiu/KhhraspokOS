#include <stdint.h>
#include <stddef.h>
#include <kpkheap.hpp>
#include <kstdio.hpp>

#ifndef DATA_STRUCTURES_HPP
#define DATA_STRUCTURES_HPP

class BitMap {
    private:
        size_t length;
        uint8_t* buffer;

    public:
        void initBitMap(size_t len);

        bool get(size_t idx);

        bool set(size_t index, bool value);

        size_t getSize();
};

template<class T>
struct node {
  node<T>* next;
  T data;
};

template<class T>
class LinkedList {
    protected:
        node<T> *head;
        node<T> *last;
        size_t size;

    public:
        LinkedList() {
            this->size = 0;
            this->head = NULL;
            this->last = NULL;
        }

        bool isEmpty() {
            return (this->size == 0 || this->head == NULL || this->last == NULL);
        }

        size_t getSize() {
            return this->size;
        }

        virtual void push(T data) {
            if(!this->head) {
                this->head = (node<T>*)KPKHEAP::kpkZalloc(sizeof(node<T>));
                this->head->data = data;
                this->head->next = NULL;
                this->last = this->head;

            } else {
                if(this->last == this->head) {
                    this->last = (node<T>*)KPKHEAP::kpkZalloc(sizeof(node<T>));
                    this->last->data = data;
                    this->last->next = NULL;
                    this->head->next = this->last;

                } else {
                    node<T>* newData = (node<T>*)KPKHEAP::kpkZalloc(sizeof(node<T>));
                    newData->data = data;
                    newData->next = NULL;
                    this->last->next = newData;
                    this->last = newData;
                }
            }

            this->size++;
        }

        virtual T* pop() {
            if (this->getSize() == 0) {
                kpwarn("List is empty!\n");

                return NULL;
            }

            T *it = this->get(this->getSize());

            this->deleteElem(this->getSize());

            this->size--;

            return it;
        }

        void deleteElem(T data) {
            if (this->isEmpty()) {
                kpwarn("List empty!\n");

                return;
            }

            node<T> *tmp = NULL;
            for (node<T> *it = this->head; it != NULL || it != this->last;
                    tmp = it, it = it->next) {
                if (it->data == data) {
                    if (tmp == NULL) {
                        this->head = it->next;

                    } else {
                        tmp->next = it->next;
                    }

                    return;
                }
            }

            kpwarn("No such element in list!\n");
        }

        void deleteElem(size_t idx) {
            if(idx == 0) {
                this->head = this->head->next;

            } else {
                node<T>* curr = this->head, *tmp;

                for(size_t i = 0; i < idx; ++i) {
                    tmp = curr;
                    curr = curr->next;
                }

                tmp->next = curr->next;
            }
        }

        T* get(int idx) {
            if(idx == 0) {
                return &this->head->data;

            } else {
                node<T>* curr = this->head;

                for(int i = 0; i < idx; ++i) {
                    curr = curr->next;
                }

                return &curr->data;
            }
        }

        T* operator[](int idx) {
            return get(idx);
        }
};


template<class T>
class Stack : public LinkedList<T> {
    public:
        void push(T data) {
            if(!this->head) {
                this->head = (node<T>*)KPKHEAP::kpkZalloc(sizeof(node<T>));
                this->head->data = data;
                this->head->next = NULL;
                this->last = this->head;

            } else {
                this->head = (node<T>*)KPKHEAP::kpkZalloc(sizeof(node<T>));
                this->head->data = data;
                this->head->next = NULL;
                this->head->next = this->last;
            }

            this->size++;
        }

        T* pop() {
            if (this->getSize() == 0) {
                kpwarn("Stack is empty!\n");

                return NULL;
            }

            T *it = this->get(0);

            this->deleteElem(0);

            this->size--;

            return it;
        }
};

template<class T>
class Queue : public LinkedList<T> {
    public:
        void push(T data) {
            if(!this->head) {
                this->head = (node<T>*)KPKHEAP::kpkZalloc(sizeof(node<T>));
                this->head->data = data;
                this->head->next = NULL;
                this->last = this->head;

            } else {
                if(this->last == this->head) {
                    this->last = (node<T>*)KPKHEAP::kpkZalloc(sizeof(node<T>));
                    this->last->data = data;
                    this->last->next = NULL;
                    this->head->next = this->last;

                } else {
                    node<T>* newData = (node<T>*)KPKHEAP::kpkZalloc(sizeof(node<T>));
                    newData->data = data;
                    newData->next = NULL;
                    this->last->next = newData;
                    this->last = newData;
                }
            }

            this->size++;
        }

        T* pop() {
            if (this->getSize() == 0) {
                kpwarn("Queue is empty!\n");

                return NULL;
            }

            T *it = this->get(0);

            this->deleteElem((size_t)0);

            this->size--;

            return it;
        }
};

#endif  /* DATA_STRUCTURES_HPP */
