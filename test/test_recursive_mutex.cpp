//
// Created by FDC on 2021/12/8.
//

#include "Mutex.h"
#include <iostream>

int main()
{
    M::recursive_mutex m;
    m.lock();
    m.lock();
    m.unlock();
    m.unlock();
    return 0;
}