//
// Created by 最上川 on 2022/3/6.
//

#include <iostream>
#include <functor.h>

void f1()
{
    std::cout << "f1\n";
}

int main()
{
    M::Functor<void(*)()> Fun1(f1);
    Fun1();

    auto f3 = [](int a) { std::cout << "a = " << a << std::endl; };
    M::Functor<void(*)(int)> Fun3(f3);
    Fun3(1);
}