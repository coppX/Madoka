//
// Created by FDC on 2021/12/8.
//

#include "Mutex.h"
#include "MThread.h"
#include <iostream>

int g_i = 0;
M::mutex m;

void safe_increment()
{
    M::lock_guard<M::mutex> lock(m);
    g_i++;
    printf("safe_increment %d\n", g_i);
}

int main()
{
    M::thread t1(safe_increment);
    M::thread t2(safe_increment);

    t1.join();
    t2.join();

    return 0;
}
