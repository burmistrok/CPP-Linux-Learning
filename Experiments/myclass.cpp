#include <iostream>
#include"myclass.hpp"

template <typename T>
myClass<T>::myClass()
{
    this->size = 0;
    this->first = nullptr;
    std::cout << "Basic constructor called" << std::endl;
}

template<typename T>
myClass<T>::myClass(T val):myClass()
{
    this->pushback(val);
    std::cout << "Advanced constructor called" << std::endl;
}

template <typename T>
myClass<T>::~myClass()
{

    std::cout << "Destructor called" << std::endl;
    while(this->size)
    {
        this->popfront();
    }
}

template<typename T>
void myClass<T>::pushback(T value)
{
    if(nullptr == this->first)
    {
        this->first = new node(value);
    }
    else
    {
        node *current = this->first;
        while(nullptr != current->nextPtr){
            current = current->nextPtr;
        }
        current->nextPtr = new node(value);
        current = current->nextPtr;
    }
    this->size++;
}

template<typename T>
void myClass<T>::pushfront(T value)
{
    if(nullptr == this->first)
    {
        this->pushback(value);
    }
    else
    {
        node *temp = new node(value, this->first);
        this->first = temp;
    }
    this->size++;
}

template<typename T>
void myClass<T>::popfront()
{
    if(nullptr != this->first){
        node *current = this->first;
        this->first = current->nextPtr;
        delete current;
        this->size--;
    }
}

template<typename T>
void myClass<T>::popback()
{
    if(0 != this->size){
        this->removeAt(this->size-1);
    }
}

template<typename T>
void myClass<T>::removeAt(const int index)
{
    if(index < size){
        if(0 == index){
            this->popfront();
        }
        else{
            node *prevnode = this->first;
            int lPos = index - 1;

            /*Find previous node, in order to write in nextPtr, value from node to be removed*/
            for(int lidx = 0; lidx < lPos; lidx++){
                prevnode = prevnode->nextPtr;
            }

            node *nodeToBeDelet = prevnode->nextPtr;
            prevnode->nextPtr = nodeToBeDelet->nextPtr;
            delete nodeToBeDelet;
            this->size--;
        }
    }
}

template<typename T>
void myClass<T>::insert(T value, const int index)
{
    if(index < size){
        if(0 != this->size){
            if(0 == index){
                this->pushfront(value);
            }
            else{
                node *prevNode = this->first;

                for(int lidx = 0; lidx < (index-1); lidx++)
                {
                    prevNode = prevNode->nextPtr;
                }

                node *actualNode = prevNode->nextPtr;
                node *temp = new node(value, actualNode);
                prevNode->nextPtr = temp;
                this->size++;
            }
        }
        else{
            this->pushfront(value);
        }
    }
}

template<typename T>
void myClass<T>::PrintHelp()
{
    std::cout << "Called from MyClass " << this << std::endl;
}

template<typename T>
T &myClass<T>::operator[](const int index)
{
    node *current = this->first;
    if(index < size){
        unsigned int curPos= 0u;
        while (curPos < size) {
            if(index == curPos){
                curPos = size;
            }
            else{
                current = current->nextPtr;
                curPos++;
            }
        }
    }
    else{
        throw(1);
    }
    return current->data;
}


