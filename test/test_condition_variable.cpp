//
// Created by FDC on 2021/12/9.
//

#include "MThread.h"
#include "Mutex.h"
#include "MCondition_variable.h"
#include <string>

M::mutex m;
M::condition_variable cv;
bool ready = false;
bool processed = false;
std::string data;

void worker_thread()
{
    // Wait until main() sends data
    M::unique_lock<M::mutex> lk(m);
    cv.wait(lk, []{return ready;});

    // after the wait, we own the lock.
    printf("Worker thread is processing data\n");
    data += " after processing";

    // Send data back to main()
    processed = true;
    printf("Worker thread signals data processing completed\n");

    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lk.unlock();
    cv.notify_one();
}

int main()
{
    M::thread worker(worker_thread);

    data = "Example data";
    // send data to the worker thread
    {
        M::lock_guard<M::mutex> lk(m);
        ready = true;
        printf("main() signals data ready for processing\n");
    }
    cv.notify_one();

    // wait for the worker
    {
        M::unique_lock<M::mutex> lk(m);
        cv.wait(lk, []{return processed;});
    }
    printf("Back in main(), data = %s", data.c_str());

    worker.join();
}