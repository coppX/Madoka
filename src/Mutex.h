//
// Created by FDC on 2021/12/7.
//

#ifndef MTHREAD_MUTEX_H
#define MTHREAD_MUTEX_H
#include "Types.h"
#include "thread_help.h"
#include <chrono>
#include <cassert>

namespace M {

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
        mutex_t m_ = MUTEXT_INITIALIZER;
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

        typedef recursive_mutex_t native_handle_type;
        native_handle_type native_handle();
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
    class unique_lock {
    public:
        typedef Mutex mutex_type;

        unique_lock() noexcept;

        explicit unique_lock(mutex_type &m);

        unique_lock(mutex_type &m, defer_lock_t) noexcept;

        unique_lock(mutex_type &m, try_to_lock_t);

        unique_lock(mutex_type &m, adopt_lock_t);

        template<typename Clock, typename Duration>
        unique_lock(mutex_type &m, const std::chrono::time_point<Clock, Duration> &abs_time);

        template<typename Rep, typename Period>
        unique_lock(mutex_type &m, const std::chrono::duration<Rep, Period> &rel_time);

        ~unique_lock();

        unique_lock(unique_lock &) = delete;

        unique_lock &operator=(unique_lock &) = delete;

        unique_lock(unique_lock &&) noexcept;

        unique_lock &operator=(unique_lock &&) noexcept;

        void lock();

        bool try_lock();

        template<typename Rep, typename Period>
        bool try_lock_for(const std::chrono::duration<Rep, Period> &rel_time);

        template<typename Clock, typename Duration>
        bool try_lock_until(const std::chrono::time_point<Clock, Duration> &abs_time);

        void unlock();

        void swap(unique_lock &u) noexcept;

        mutex_type *release() noexcept;

        bool owns_lock() const noexcept;

        explicit  operator bool() const noexcept;

        mutex_type *mutex() const noexcept;
    };

};
#endif //MTHREAD_MUTEX_H
