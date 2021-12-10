//
// Created by FDC on 2021/12/10.
//

#include <iostream>
#include "Mutex.h"
#include "MThread.h"
#include <chrono>
#include <thread>
#include <vector>
#include <sstream>
using namespace std::chrono_literals;

M::mutex cout_mutex; // control access to std::cout
M::timed_mutex mutex;

void job(int id)
{
    std::ostringstream stream;

    for (int i = 0; i < 3; ++i) {
        if (mutex.try_lock_for(100ms)) {
            stream << "success ";
            std::this_thread::sleep_for(100ms);
            mutex.unlock();
        } else {
            stream << "failed ";
        }
        std::this_thread::sleep_for(100ms);
    }

    M::lock_guard<M::mutex> lock{cout_mutex};
    std::cout << "[" << id << "] " << stream.str() << "\n";
}

int main()
{
    std::vector<M::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(job, i);
    }

    for (auto& i: threads) {
        i.join();
    }
}