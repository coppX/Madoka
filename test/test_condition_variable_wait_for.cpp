//
// Created by 最上川 on 2021/12/9.
//

#include <iostream>
#include "MCondition_variable.h"
#include "Mutex.h"
#include "MThread.h"
#include <chrono>
#include <thread>
using namespace std::chrono_literals;

M::condition_variable cv;
M::mutex cv_m;
int i;

void waits(int idx)
{
    M::unique_lock<M::mutex> lk(cv_m);
    if(cv.wait_for(lk, idx*100ms, []{return i == 1;}))
        std::cerr << "Thread " << idx << " finished waiting. i == " << i << '\n';
    else
        std::cerr << "Thread " << idx << " timed out. i == " << i << '\n';
}

void signals()
{
    std::this_thread::sleep_for(120ms);
    std::cerr << "Notifying...\n";
    cv.notify_all();
    std::this_thread::sleep_for(100ms);
    {
        M::lock_guard<M::mutex> lk(cv_m);
        i = 1;
    }
    std::cerr << "Notifying again...\n";
    cv.notify_all();
}

int main()
{
    M::thread t1(waits, 1), t2(waits, 2), t3(waits, 3), t4(signals);
    t1.join(), t2.join(), t3.join(), t4.join();
}