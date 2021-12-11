//
// Created by 最上川 on 2021/12/11.
//

#ifndef MTHREAD_MSHARED_MUTEX_H
#define MTHREAD_MSHARED_MUTEX_H
namespace M
{
    class shared_mutex
    {
    public:
        shared_mutex();
        ~shared_mutex();

        shared_mutex(const shared_mutex&) = delete;
        shared_mutex& operator=(const shared_mutex&) = delete;

        void lock();
        bool try_lock();
        void unlock();

        void lock_shared();
        bool try_lock_shared();
        void unlock_shared();
    };
}
#endif //MTHREAD_MSHARED_MUTEX_H
