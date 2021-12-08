//
// Created by FDC on 2021/12/7.
//

#ifndef MTHREAD_MUTEX_H
#define MTHREAD_MUTEX_H
#include "Types.h"
#include <chrono>

namespace M {

    class mutex {
    public:
        constexpr mutex() noexcept {

        }

        ~mutex() {
#if defined(__APPLE__) || (__linux)
            pthread_mutex_destroy(&_m);
#elif defined(_WIN32)

#endif
        }

        mutex(const mutex &) = delete;

        mutex &operator=(const mutex &) = delete;

        void lock() {
            int ec = -1;
#if defined (__APPLE__) || defined (__linux__)
            ec = pthread_mutex_lock(&_m);
#elif defined (_WIN32)
#endif
            if (0 != ec) {
                printf("mutex lock failed");
                std::abort();
            }
        }

        bool try_lock() {
            int ec = -1;
#if defined(__APPLE__) || defined(__linux__)
            ec = pthread_mutex_trylock(&_m);
#elif defined(_WIN32)
#endif
            return 0 == ec;
        }

        void unlock() {
            int ec = -1;
#if defined(__APPLE__) || defined (__linux__)
            ec = pthread_mutex_unlock(&_m);
#elif defined(_WIN32)
#endif
            if (0 != ec) {
                printf("mutex unlock failed");
            }
        }

#if defined (__APPLE__) || defined (__linux__)
        typedef pthread_mutex_t mutex_t;
#elif defined (_WIN32)
        typedef void *mutex_t;
#endif
        typedef mutex_t native_handle_type;

        native_handle_type native_handle();

    private:
        mutex_t _m = MUTEXT_INITIALIZER;
    };

    class recursive_mutex {
    public:
        recursive_mutex();

        ~recursive_mutex();

        recursive_mutex(const recursive_mutex &) = delete;

        recursive_mutex &operator=(const recursive_mutex &) = delete;

        void lock();

        bool try_lock();

        void unlock();

#if defined(__APPLE__) || defined(__linux__)
        typedef pthread_mutex_t native_handle_type;
#elif defined(_WIN32)
        typedef void *native_handle_type;
#endif

        native_handle_type native_handle();
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
