//
// Created by FDC on 2021/12/7.
//

#ifndef MTHREAD_MUTEX_H
#define MTHREAD_MUTEX_H
#include "Types.h"
#include "thread_help.h"
#include <chrono>
#include <cassert>
#include <algorithm>

namespace M {

    struct defer_lock_t {
        explicit defer_lock_t() = default;
    };

    struct try_to_lock_t {
        explicit try_to_lock_t() = default;
    };

    struct adopt_lock_t {
        explicit adopt_lock_t() = default;
    };


    template<typename Mutex>
    class lock_guard
    {
    public:
        typedef Mutex mutex_type;
        explicit lock_guard(mutex_type &m)
            : m_(m)
        {
            m_.lock();
        }

        lock_guard(mutex_type& m, adopt_lock_t)
            : m_(m)
        {

        }

        ~lock_guard()
        {
            m_.unlock();
        }

        lock_guard(const lock_guard&) = delete;
        lock_guard& operator=(const lock_guard&) = delete;

    private:
        mutex_type& m_;
    };

    class mutex {
    public:
        constexpr mutex() = default;

        ~mutex()
        {
            assert(0 == mutex_destroy(&m_));
        }

        mutex(const mutex &) = delete;

        mutex &operator=(const mutex &) = delete;

        void lock()
        {
            if (0 != mutex_lock(&m_)) {
                printf("mutex lock failed");
                std::abort();
            }
        }

        bool try_lock()
        {
           return mutex_trylock(&m_);
        }

        void unlock()
        {
            if (0 != mutex_unlock(&m_)) {
                printf("mutex unlock failed");
            }
        }


        typedef mutex_t native_handle_type;
        native_handle_type native_handle()
        {
            return m_;
        }

    private:
        mutex_t m_ = MUTEX_INITIALIZER;
    };

    class recursive_mutex {
    public:
        recursive_mutex()
        {
            if (0 != recursive_mutex_init(&m_))
            {
                printf("recursive_mutex constructor failed");
            }
        }

        ~recursive_mutex()
        {
            if (0 != recursive_mutex_destroy(&m_))
            {
                printf("recursive_mutex destructor failed");
            }
        }

        recursive_mutex(const recursive_mutex &) = delete;

        recursive_mutex &operator=(const recursive_mutex &) = delete;

        void lock()
        {
            if (0 != recursive_mutex_lock(&m_))
            {
                printf("recursive_mutex_lock failed");
            }
        }

        bool try_lock()
        {
            return recursive_mutex_trylock(&m_);
        }

        void unlock()
        {
            if (0 != recursive_mutex_unlock(&m_))
            {
                printf("recursive_mutex_unlock failed");
            }
        }

        typedef recursive_mutex_t* native_handle_type;
        native_handle_type native_handle()
        {
            return &m_;
        }
    private:
        recursive_mutex_t m_;
    };

    class timed_mutex {
    public:
        timed_mutex();

        ~timed_mutex();

        timed_mutex(const timed_mutex &) = delete;

        timed_mutex &operator=(const timed_mutex &) = delete;

        void lock();

        bool try_lock();

        template<typename Rep, typename Period>
        bool try_lock_for(const std::chrono::duration<Rep, Period> &rel_time);

        template<typename Clock, typename Duration>
        bool try_lock_until(const std::chrono::time_point<Clock, Duration> &abs_time);

        void unlock();
    };

    class recursive_timed_mutex {
    public:
        recursive_timed_mutex();

        ~recursive_timed_mutex();

        recursive_timed_mutex(const recursive_timed_mutex &) = delete;

        recursive_timed_mutex &operator=(const recursive_timed_mutex &) = delete;

        void lock();

        bool try_lock();

        template<typename Rep, typename Period>
        bool try_lock_for(const std::chrono::duration<Rep, Period> &rel_time);

        template<typename Clock, typename Duration>
        bool try_lock_until(const std::chrono::time_point<Clock, Duration> &abs_time);

        void unlock();
    };

    template<typename Mutex>
    class unique_lock {
    public:
        typedef Mutex mutex_type;

        unique_lock() noexcept : m_(nullptr), owns_(false) {}

        explicit unique_lock(mutex_type &m)
            : m_(std::addressof(m)), owns_(true)
        {
            m_->lock();
        }

        unique_lock(mutex_type &m, defer_lock_t) noexcept
            : m_(std::addressof(m)), owns_(false) {}

        unique_lock(mutex_type &m, try_to_lock_t)
            : m_(std::addressof(m)), owns_(m.try_lock()) {}

        unique_lock(mutex_type &m, adopt_lock_t)
            : m_(std::addressof(m)), owns_(true) {}

        template<typename Clock, typename Duration>
        unique_lock(mutex_type &m, const std::chrono::time_point<Clock, Duration> &abs_time)
            : m_(std::addressof(m)), owns_(m.try_lock_until(abs_time)) {}

        template<typename Rep, typename Period>
        unique_lock(mutex_type &m, const std::chrono::duration<Rep, Period> &rel_time)
            : m_(std::addressof(m)), owns_(m.try_lock_for(rel_time)) {}

        ~unique_lock()
        {
            if (owns_)
                m_->unlock();
        }

        unique_lock(unique_lock &) = delete;

        unique_lock &operator=(unique_lock &) = delete;

        unique_lock(unique_lock&& other) noexcept
            : m_(other.m_), owns_(other.owns_)
        {
            other.m_ = nullptr;
            other.owns_ = false;
        }

        unique_lock &operator=(unique_lock&& other) noexcept
        {
            if (owns_)
                m_->unlock();
            m_ = other.m_;
            owns_ = other.owns_;
            other.m_ = nullptr;
            other.owns_ = false;
            return *this;
        }

        void lock()
        {
            static_assert(m_, "unique_lock::try_lock: reference null mutex");
            static_assert(!owns_, "unique_lock::try_lock: already locked");
            m_->lock();
            owns_ = true;
        }

        bool try_lock()
        {
            static_assert(m_, "unique_lock::try_lock: reference null mutex");
            static_assert(!owns_, "unique_lock::try_lock: already locked");
            owns_ = m_->try_lock();
            return owns_;
        }

        template<typename Rep, typename Period>
        bool try_lock_for(const std::chrono::duration<Rep, Period> &rel_time)
        {
            static_assert(m_, "unique_lock::try_lock: reference null mutex");
            static_assert(!owns_, "unique_lock::try_lock: already locked");
            owns_ = m_->try_lock_for(rel_time);
            return owns_;
        }

        template<typename Clock, typename Duration>
        bool try_lock_until(const std::chrono::time_point<Clock, Duration> &abs_time)
        {
            static_assert(m_, "unique_lock::try_lock: reference null mutex");
            static_assert(!owns_, "unique_lock::try_lock: already locked");
            owns_ = m_->try_lock_until(abs_time);
            return owns_;
        }

        void unlock()
        {
            if (owns_)
            {
                m_->unlock();
                owns_ = false;
            }
        }

        void swap(unique_lock &other) noexcept
        {
            std::swap(m_, other.m_);
            std::swap(owns_, other.owns_);
        }

        mutex_type *release() noexcept
        {
            mutex_type* m = m_;
            m_ = nullptr;
            owns_ = false;
            return m;
        }

        bool owns_lock() const noexcept { return owns_; }

        explicit  operator bool() const noexcept { return owns_; }

        mutex_type *mutex() const noexcept { return m_; }
    private:
        mutex_type* m_;
        bool owns_;
    };

};
#endif //MTHREAD_MUTEX_H
