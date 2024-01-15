#ifndef MYCLASS_HPP
#define MYCLASS_HPP


template <typename T>
class myClass
{
public:
    myClass();
    myClass(T val);

    ~myClass();

    void pushback(T value);
    void pushfront(T value);
    void popfront();
    void popback();
    void removeAt(const int index);
    void insert(T value, const int index);
    int getSize(void) { return size;}
    void PrintHelp();
    T& operator[](const int index);

private:
    class node{
    public:
        T data;
        node* nextPtr;

        node(T value = T(0), node* nextptr = nullptr)
        {
            this->data = value;
            this->nextPtr = nextptr;
        }
    };
    int size;
    node *first;
};

#endif // MYCLASS_HPP
