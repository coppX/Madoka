//
// Created by FDC on 2021/12/7.
//

#ifndef MADOKA_MUTEX_H
#define MADOKA_MUTEX_H
#include "Types.h"
#include "thread_header.h"
#include <cassert>
#include <algorithm>
#include "MCondition_variable.h"

namespace M {

    template<typename Lockable0, typename Lockable1>
    void lock(Lockable0& l0, Lockable1& l1)
    {
        while (true)
        {
            {
                unique_lock<Lockable0> lk0(l0);
                if (l1.try_lock())
                {
                    lk0.release();
                    break;
                }
            }
            thread_yield();

            {
                unique_lock<Lockable1> lk1(l1);
                if (l0.try_lock())
                {
                    lk1.release();
                    break;
                }
            }
            thread_yield();
        }
    }

    template<typename Lockable0, typename Lockable1, typename... LockableN>
    void lock(Lockable0& l0, Lockable1& l1, LockableN&... lN)
    {
        while(true)
        {
            unique_lock<Lockable0> lk0(l0);
            if (-1 == try_lock(l1, lN...))
            {
                lk0.release();
                return;
            }
            thread_yield();
        }
    }

    template<typename Lockable0, typename Lockable1>
    int try_lock(Lockable0& l0, Lockable1& l1)
    {
        unique_lock<Lockable1> lk(l0, try_to_lock);
        if (lk.owns_lock())
        {
            if (l1.try_lock())
            {
                lk.release();
                return -1;
            }
            else
            {
                return 1;
            }
        }
    }

    template<typename Lockable0, typename Lockable1, typename ... LockableN>
    int try_lock(Lockable0& lock0, Lockable1& lock1, LockableN&... lockN)
    {
        int res = 0;
        unique_lock<Lockable1> lk(lock0, try_to_lock);
        if (lk.owns_lock())
        {
            res = try_lock(lock1, lockN...);
            if (res == -1)
                lk.release();
            else
                ++res;
        }
        return res;
    }

    template<typename Mutex>
    lock_guard<Mutex>::lock_guard(mutex_type &m)
        : m_(m)
    {
        m.lock();
    }

    template<typename Mutex>
    lock_guard<Mutex>::lock_guard(mutex_type &m, M::adopt_lock_t)
        : m_(m)
    {

    }

    template<typename Mutex>
    lock_guard<Mutex>::~lock_guard()
    {
        m_.unlock();
    }


    mutex::~mutex()
    {
        if (0 != mutex_destroy(&m_))
        {
            printf("mutex destroy failed\n");
        }
    }

    void mutex::lock()
    {
        if (0 != mutex_lock(&m_)) {
            printf("mutex lock failed\n");
            std::abort();
        }
    }

    bool mutex::try_lock()
    {
        return mutex_trylock(&m_);
    }

    void mutex::unlock()
    {
        if (0 != mutex_unlock(&m_)) {
            printf("mutex unlock failed\n");
        }
    }

    mutex::native_handle_type mutex::native_handle()
    {
        return &m_;
    }

    recursive_mutex::recursive_mutex()
    {
        if (0 != recursive_mutex_init(&m_))
        {
            printf("recursive_mutex constructor failed\n");
        }
    }

    recursive_mutex::~recursive_mutex()
    {
        if (0 != recursive_mutex_destroy(&m_))
        {
            printf("recursive_mutex destructor failed\n");
        }
    }

    void recursive_mutex::lock()
    {
        if (0 != recursive_mutex_lock(&m_))
        {
            printf("recursive_mutex_lock failed\n");
        }
    }

    bool recursive_mutex::try_lock()
    {
        return recursive_mutex_trylock(&m_);
    }

    void recursive_mutex::unlock()
    {
        if (0 != recursive_mutex_unlock(&m_))
        {
            printf("recursive_mutex_unlock failed\n");
        }
    }

    recursive_mutex::native_handle_type recursive_mutex::native_handle()
    {
        return &m_;
    }

    timed_mutex::timed_mutex()
        : locked_(false)
    {}

    timed_mutex::~timed_mutex()
    {
        lock_guard<mutex> lk(m_);
    }

    void timed_mutex::lock()
    {
        unique_lock<mutex> lk(m_);
        while (locked_)
        {
            cv_.wait(lk);
        }
        locked_ = true;
    }

    bool timed_mutex::try_lock()
    {
        unique_lock<mutex> lk(m_, try_to_lock);
        if (lk.owns_lock())
        {
            locked_ = true;
            return true;
        }
        return false;
    }

    template<typename Rep, typename Period>
    bool timed_mutex::try_lock_for(const duration<Rep, Period> &rel_time)
    {
        return try_lock_until(steady_clock::now() + rel_time);
    }

    template<typename Clock, typename Duration>
    bool timed_mutex::try_lock_until(const time_point <Clock, Duration> &abs_time)
    {
        unique_lock<mutex> lk(m_);
        bool no_timeout = Clock::now() < abs_time;
        while (locked_ && no_timeout)
        {
            no_timeout = cv_.template wait_until<Clock, Duration>(lk, abs_time) == cv_status::no_timeout;
        }
        if (!locked_)
        {
            locked_ = true;
            return true;
        }
        return false;
    }

    void timed_mutex::unlock()
    {
        lock_guard<mutex> lk(m_);
        locked_ = false;
        cv_.notify_one();
    }

    template<typename Mutex>
    unique_lock<Mutex>::~unique_lock()
    {
        if (owns_)
            m_->unlock();
    }

    template<typename Mutex>
    unique_lock<Mutex>::unique_lock(unique_lock&& other) noexcept
        : m_(other.m_), owns_(other.owns_)
    {
        other.m_ = nullptr;
        other.owns_ = false;
    }

    template<typename Mutex>
    unique_lock<Mutex>& unique_lock<Mutex>::operator=(unique_lock&& other) noexcept
    {
        if (owns_)
            m_->unlock();
        m_ = other.m_;
        owns_ = other.owns_;
        other.m_ = nullptr;
        other.owns_ = false;
        return *this;
    }

    template<typename Mutex>
    void unique_lock<Mutex>::lock()
    {
        if(!m_)
        {
            printf("unique_lock::lock: reference null mutex\n");
            return;
        }
        if(owns_)
        {
            printf("unique_lock::lock: already locked\n");
            return;
        }
        m_->lock();
        owns_ = true;
    }

    template<typename Mutex>
    bool unique_lock<Mutex>::try_lock()
    {
        if(!m_)
        {
            printf("unique_lock::try_lock: reference null mutex\n");
            return false;
        }
        if(owns_)
        {
            printf("unique_lock::try_lock: already locked\n");
            return false;
        }
        owns_ = m_->try_lock();
        return owns_;
    }

    template<typename Mutex>
    template<typename Rep, typename Period>
    bool unique_lock<Mutex>::try_lock_for(const std::chrono::duration<Rep, Period> &rel_time)
    {
        if(!m_)
        {
            printf("unique_lock::try_lock_for: reference null mutex\n");
            return false;
        }
        if(owns_)
        {
            printf("unique_lock::try_lock_for: already locked\n");
            return false;
        }
        owns_ = m_->try_lock_for(rel_time);
        return owns_;
    }

    template<typename Mutex>
    template<typename Clock, typename Duration>
    bool unique_lock<Mutex>::try_lock_until(const std::chrono::time_point<Clock, Duration> &abs_time)
    {
        if(!m_)
        {
            printf("unique_lock::try_lock_until: reference null mutex\n");
            return false;
        }
        if(owns_)
        {
            printf("unique_lock::try_lock_until: already locked\n");
            return false;
        }
        owns_ = m_->try_lock_until(abs_time);
        return owns_;
    }

    template<typename Mutex>
    void unique_lock<Mutex>::unlock()
    {
        if (owns_)
        {
            m_->unlock();
            owns_ = false;
        }
    }

    template<typename Mutex>
    void unique_lock<Mutex>::swap(unique_lock &other) noexcept
    {
        std::swap(m_, other.m_);
        std::swap(owns_, other.owns_);
    }

    template<typename Mutex>
    typename unique_lock<Mutex>::mutex_type*
    unique_lock<Mutex>::release() noexcept
    {
        mutex_type* m = m_;
        m_ = nullptr;
        owns_ = false;
        return m;
    }

};
#endif //MADOKA_MUTEX_H
