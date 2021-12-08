//
// Created by FDC on 2021/12/8.
//

#include "Mutex.h"
#include "MThread.h"
#include <string>


class X{
    M::recursive_mutex m;
    std::string s;
public:
    void fun1()
    {
        m.lock();
        s = "fun1";
        printf("in fun1, shared variable is now %s\n", s.c_str());
        m.unlock();
    }

    void fun2()
    {
        m.lock();
        s = "fun2";
        printf("in fun2, shared variable is now %s\n", s.c_str());
        fun1();
        m.unlock();
    }
};
int main()
{
    X x;
    M::thread t1(&X::fun1, &x);
    M::thread t2(&X::fun2, &x);

    t1.join();
    t2.join();

    //x.fun1();
    //x.fun2();
    return 0;
}