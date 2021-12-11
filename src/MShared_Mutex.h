//
// Created by 最上川 on 2021/12/11.
//

#ifndef MTHREAD_MSHARED_MUTEX_H
#define MTHREAD_MSHARED_MUTEX_H
#include "thread_header.h"

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

    class shared_timed_mutex
    {
    public:
        shared_timed_mutex();
        ~shared_timed_mutex();

        shared_timed_mutex(const shared_timed_mutex&) = delete;
        shared_timed_mutex& operator=(const shared_timed_mutex&) = delete;

        void lock();
        void try_lock();
        template<typename Rep, typename Period>
        bool try_lock_for(const duration<Rep, Period>& rel_time);

        template<typename Clock, typename Duration>
        bool try_lock_until(const time_point<Clock, Duration>& abs_time);

        void lock_shared();
        bool try_lock_shared();

        template<typename Rep, typename Period>
        bool try_lock_shared_for(const duration<Rep, Period>& rel_time);

        template<typename Clock, typename Period>
        bool try_lock_shared_until(const time_point<Clock, Period>& abs_time);

        void unlock_shared();
    };
}
#endif //MTHREAD_MSHARED_MUTEX_H
