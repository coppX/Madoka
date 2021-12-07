#include "MThread.h"
#include <iostream>
#include <functional>

int f1(int& a, const int& b)
{
    printf("test normal function in thread %d, %d\n", a, b);
    return 0;
}
int main()
{
    MThread t1([](int a, int b){ printf("test lambda in thread %d, %d\n", a, b); }, 1, 2);
    t1.join();

    int var1 = 2, var2 = 3;
    MThread t2(f1, std::ref(var1), std::cref(var2));
    t2.join();

    return 0;
}