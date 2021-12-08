//
// Created by 最上川 on 2021/12/8.
//

#include "Mutex.h"
#include "MThread.h"
#include <functional>

struct Box {
    explicit Box(int num) : num_things(num) {}
    int num_things;
    M::mutex m;
};

void transfer(Box &from, Box &to, int num)
{
    M::unique_lock<M::mutex> lock1(from.m, M::defer_lock);
    M::unique_lock<M::mutex> lock2(to.m, M::defer_lock);

    lock1.lock();
    lock2.lock();

    from.num_things -= num;
    to.num_things += num;
}

int main()
{
    Box acc1(100);
    Box acc2(50);

    M::thread t1(transfer, std::ref(acc1), std::ref(acc2), 10);
    M::thread t2(transfer, std::ref(acc1), std::ref(acc2), 5);

    t1.join();
    t2.join();
    printf("acc1.num_things = %d, acc2.num_things = %d", acc1.num_things, acc2.num_things);
}