#include "MThread"
#include <iostream>

int main()
{
    MThread t1([](int a, int b){ printf("test thread lambda %d, %d", a, b); }, 1, 2);
    t1.join();
    return 0;
}