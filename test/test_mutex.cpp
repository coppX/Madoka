//
// Created by 最上川 on 2021/12/7.
//

#include <iostream>
#include "Mutex.h"

int main()
{
    Mutex m;
    m.lock();
    printf("m.try_lock() = %d", m.try_lock());
    return 0;
}