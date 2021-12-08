//
// Created by 最上川 on 2021/12/7.
//

#include <iostream>
#include "Mutex.h"

int main()
{
    M::mutex m;
    m.lock();
    printf("m.try_lock() = %d\n", m.try_lock());
    m.unlock();

    return 0;
}