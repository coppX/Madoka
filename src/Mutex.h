//
// Created by FDC on 2021/12/7.
//

#ifndef MTHREAD_MUTEX_H
#define MTHREAD_MUTEX_H
#include "Types.h"
#include <chrono>

class Mutex
{
public:
    constexpr Mutex() noexcept;
    ~Mutex();
    Mutex(const Mutex&) = delete;
    Mutex& operator=(const Mutex&) = delete;

    void lock();
    bool try_lock();
    void unlock();

#if defined (__APPLE__) || defined (__linux__)
    typedef pthread_mutex_t* native_handle_type;
#elif defined (_WIN32)
    typedef void* native_handle_type;
#endif
    native_handle_type native_handle();
};

class Recursive_mutex
{
public:
    Recursive_mutex();
    ~Recursive_mutex();

    Recursive_mutex(const Recursive_mutex&) = delete;
    Recursive_mutex& operator=(const Recursive_mutex&) = delete;

    void lock();
    bool try_lock();
    void unlock();

#if defined(__APPLE__) || defined(__linux__)
    typedef pthread_mutex_t native_handle_type;
#elif defined(_WIN32)
    typedef void* native_handle_type;
#endif
    native_handle_type native_handle();
};

class Timed_mutex
{
public:
    Timed_mutex();
    ~Timed_mutex();

    Timed_mutex(const Timed_mutex&) = delete;
    Timed_mutex& operator=(const Timed_mutex&) = delete;

    void lock();
    bool try_lock();

    template<typename Rep, typename Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& rel_time);

    template<typename Clock, typename Duration>
    bool try_lock_until(const std::chrono::time_point<Clock, Duration>& abs_time);

    void unlock();
};

class Recursive_timed_mutex
{
public:
    Recursive_timed_mutex();
    ~Recursive_timed_mutex();

    Recursive_timed_mutex(const Recursive_timed_mutex&) = delete;
    Recursive_timed_mutex& operator=(const Recursive_timed_mutex&) = delete;

    void lock();
    bool try_lock();

    template<typename Rep, typename Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& rel_time);

    template<typename Clock, typename Duration>
    bool try_lock_until(const std::chrono::time_point<Clock, Duration>& abs_time);

    void unlock();
};

struct defer_lock_t { explicit defer_lock_t() = default; };
struct try_to_lock_t { explicit try_to_lock_t() = default; };
struct adopt_lock_t { explicit adopt_lock_t() = default; };

template<typename Mutex>
class MUnique_lock
{
public:
    typedef Mutex mutex_type;
    MUnique_lock() noexcept;
    explicit MUnique_lock(mutex_type& m);

    MUnique_lock(mutex_type& m, defer_lock_t) noexcept;
    MUnique_lock(mutex_type& m, try_to_lock_t);
    MUnique_lock(mutex_type& m, adopt_lock_t);

    template<typename Clock, typename Duration>
    MUnique_lock(mutex_type& m, const std::chrono::time_point<Clock, Duration>& abs_time);
};
#endif //MTHREAD_MUTEX_H
