
#include"myclass.hpp"
#include "myclass.cpp"
#include <iostream>


template<typename Tl>
class MyLocalClass: myClass<Tl>
{
public:
    void PrintHelp(){
        std::cout << "Called from MyLocalClass: " << this << std::endl;
    }
};

using namespace std;

int main()
{
    int Val;
    myClass<int> test(2);
    test.pushback(3);
    test.pushfront(1);
    cout << "Hello World!" << endl;

    test.insert(5, 2);
    test.pushfront(100);
    test.PrintHelp();

    MyLocalClass<int> test2;
    test2.PrintHelp();
    for(int i = 0; i < (test.getSize() + 2); i++)
    {
        try {
            int temp = test[i];
            cout << "A[" << i << "] is:" << temp << endl;
        } catch (int i) {
            cout << "The error occured : " << i << endl;
        }
    }
    return 0;
}
