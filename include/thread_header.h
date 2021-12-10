//
// Created by FDC on 2021/12/10.
//

#ifndef MTHREAD_THREAD_HEADER_H
#define MTHREAD_THREAD_HEADER_H

#include "thread_help.h"
#include <chrono>

namespace M
{
    using namespace std::chrono;

    struct defer_lock_t { explicit defer_lock_t() = default;};
    struct try_to_lock_t { explicit try_to_lock_t() = default; };
    struct adopt_lock_t { explicit adopt_lock_t() = default; };

    constexpr defer_lock_t defer_lock {};
    constexpr try_to_lock_t try_to_lock {};
    constexpr adopt_lock_t adopt_lock {};

    enum class cv_status {
        no_timeout, timeout
    };

    template<typename Mutex>
    class lock_guard
    {
    public:
        typedef Mutex mutex_type;
        explicit lock_guard(mutex_type &m);
        lock_guard(mutex_type& m, adopt_lock_t);

        ~lock_guard();

        lock_guard(const lock_guard&) = delete;
        lock_guard& operator=(const lock_guard&) = delete;
    private:
        mutex_type& m_;
    };

    class mutex {
    public:
        constexpr mutex() = default;

        ~mutex();

        mutex(const mutex &) = delete;
        mutex &operator=(const mutex &) = delete;

        void lock();
        bool try_lock();
        void unlock();

        typedef mutex_t* native_handle_type;
        native_handle_type native_handle();
    private:
        mutex_t m_ = MUTEX_INITIALIZER;
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

        typedef recursive_mutex_t* native_handle_type;
        native_handle_type native_handle();
    private:
        recursive_mutex_t m_;
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

        unique_lock() noexcept :  m_(nullptr), owns_(false) {}
        explicit unique_lock(mutex_type &m): m_(std::addressof(m)), owns_(true)
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
        unique_lock(mutex_type &m, const time_point<Clock, Duration> &abs_time)
                : m_(std::addressof(m)), owns_(m.try_lock_until(abs_time)) {}

        template<typename Rep, typename Period>
        unique_lock(mutex_type &m, const duration<Rep, Period> &rel_time)
                : m_(std::addressof(m)), owns_(m.try_lock_for(rel_time)) {}

        ~unique_lock();

        unique_lock(unique_lock &) = delete;
        unique_lock &operator=(unique_lock &) = delete;

        unique_lock(unique_lock&& other) noexcept;
        unique_lock &operator=(unique_lock&& other) noexcept;

        void lock();
        bool try_lock();

        template<typename Rep, typename Period>
        bool try_lock_for(const duration<Rep, Period> &rel_time);

        template<typename Clock, typename Duration>
        bool try_lock_until(const time_point<Clock, Duration> &abs_time);

        void unlock();

        void swap(unique_lock &other) noexcept;
        mutex_type *release() noexcept;

        bool owns_lock() const noexcept { return owns_; }

        explicit  operator bool() const noexcept { return owns_; }

        mutex_type *mutex() const noexcept { return m_; }
    private:
        mutex_type* m_;
        bool owns_;
    };



    class condition_variable {
    public:
        condition_variable() = default;

        ~condition_variable();

        condition_variable(condition_variable &) = delete;
        condition_variable &operator=(condition_variable &) = delete;

        void notify_all() noexcept;
        void notify_one() noexcept;

        void wait(unique_lock<mutex> &lock);

        template<typename Predicate>
        void wait(unique_lock<mutex> &lock, Predicate pred);

        template<typename Clock, typename Duration>
        cv_status
        wait_until(unique_lock<mutex> &lock, const time_point<Clock, Duration> &abs_time);

        template<typename Clock, typename Duration, typename Predicate>
        bool
        wait_until(unique_lock<mutex> &lock, const time_point<Clock, Duration> &abs_time, Predicate pred);

        template<typename Rep, typename Period>
        cv_status
        wait_for(unique_lock<mutex> &lock, const duration<Rep, Period> &rel_time);

        template<typename Rep, typename Period, typename Predicate>
        bool
        wait_for(unique_lock<mutex> &lock, const duration<Rep, Period> &rel_time, Predicate pred);

        typedef cond_t* native_handle_type;
        native_handle_type native_handle() { return &cv_; }
    private:

        void _do_timed_wait(unique_lock<mutex>& lk, time_point<system_clock, nanoseconds> tp)
        {
            if (!lk.owns_lock())
            {
                printf("condition_variable time_wait: mutex not locked\n");
                return;
            }
            nanoseconds d = tp.time_since_epoch();
            timespec_t ts;
            seconds s = duration_cast<seconds>(d);
            ts.tv_sec = static_cast<decltype(ts.tv_sec)>(s.count());
            ts.tv_nsec = static_cast<decltype(ts.tv_nsec)>((d - s).count());
            int ec = condition_variable_timedwait(&cv_, lk.mutex()->native_handle(), &ts);
            if (ec && ec != ETIMEDOUT)
            {
                printf("condition_variable timed_wait failed\n");
            }
        }

        template<typename Clock>
        void _do_timed_wait(unique_lock<mutex>& lock, time_point<Clock, nanoseconds> tp) noexcept
        {
            wait_for(lock, tp - Clock::now());
        }

        cond_t cv_ = COND_INITIALIZER;
    };

    class condition_variable_any {
    public:
        condition_variable_any();

        ~condition_variable_any();

        condition_variable_any(const condition_variable_any &) = delete;

        condition_variable_any &operator=(const condition_variable_any &) = delete;

        void notify_all() noexcept;

        void notify_one() noexcept;

        template<typename Lock>
        void wait(Lock &lock);

        template<typename Lock, typename Predicate>
        void wait(Lock &lock, Predicate pred);

        template<typename Lock, typename Clock, typename Duration>
        cv_status wait_until(Lock &lock, const std::chrono::time_point<Clock, Duration> &abs_time);

        template<typename Lock, typename Clock, typename Duration, typename Predicate>
        bool wait_until(Lock &lock, const std::chrono::time_point<Clock, Duration> &abs_time, Predicate pred);

        template<typename Lock, typename Rep, typename Period>
        cv_status wait_for(Lock &lock, const std::chrono::duration<Rep, Period> &rel_time);

        template<typename Lock, typename Rep, typename Period, typename Predicate>
        bool wait_for(Lock &lock, const std::chrono::duration<Rep, Period> &rel_time, Predicate pred);
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
        bool try_lock_for(const duration<Rep, Period> &rel_time);

        template<typename Clock, typename Duration>
        bool try_lock_until(const time_point<Clock, Duration> &abs_time);

        void unlock();
    private:
        mutex m_;
        condition_variable cv_;
        bool locked_;
    };
};

#endif //MTHREAD_THREAD_HEADER_H
