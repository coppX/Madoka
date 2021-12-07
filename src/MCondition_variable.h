//
// Created by FDC on 2021/12/7.
//

#ifndef MTHREAD_MCONDITION_VARIABLE_H
#define MTHREAD_MCONDITION_VARIABLE_H
#include "Types.h"
#include "Mutex.h"
enum class cv_status {no_timeout, timeout};
class MCondition_variable
{
public:
    MCondition_variable();
    ~MCondition_variable();

    MCondition_variable(MCondition_variable&) = delete;
    MCondition_variable& operator=(MCondition_variable&) = delete;

    void notify_all() noexcept;
    void notify_one() noexcept;

    void wait(MUnique_lock<Mutex>& lock);

    template<typename Predicate>
    void wait(MUnique_lock<Mutex>& lock, Predicate pred);

    template<typename Clock, typename Duration>
    cv_status wait_until(MUnique_lock<Mutex>& lock, const std::chrono::time_point<Clock, Duration>& abs_time);

    template<typename Clock, typename Duration, typename Predicate>
    bool wait_util(MUnique_lock<Mutex>& lock, const std::chrono::time_point<Clock, Duration>& abs_time, Predicate pred);

    template<typename Rep, typename Period>
    cv_status wait_for(MUnique_lock<Mutex>& lock, const std::chrono::duration<Rep, Period>& rel_time);

    template<typename Rep, typename Period, typename Predicate>
    bool wait_for(MUnique_lock<Mutex>& lock, const std::chrono::duration<Rep, Period>& rel_time, Predicate pred);

#if defined (__APPLE__) || defined (__linux__)
    typedef pthread_cond_t* native_handle_type;
#elif defined (_WIN32)
    typedef void* native_handle_type;
#endif
    native_handle_type native_handle();
};

class MCondition_variable_any
{
public:
    MCondition_variable_any();
    ~MCondition_variable_any();

    MCondition_variable_any(const MCondition_variable_any&) = delete;
    MCondition_variable_any& operator=(const MCondition_variable_any&) = delete;

    void notify_all() noexcept;
    void notify_one() noexcept;

    template<typename Lock>
    void wait(Lock& lock);

    template<typename Lock, typename Predicate>
    void wait(Lock& lock, Predicate pred);

    template<typename Lock, typename Clock, typename Duration>
    cv_status wait_until(Lock& lock, const std::chrono::time_point<Clock, Duration>& abs_time);

    template<typename Lock, typename Clock, typename Duration, typename Predicate>
    bool wait_until(Lock& lock, const std::chrono::time_point<Clock, Duration>& abs_time, Predicate pred);

    template<typename Lock, typename Rep, typename Period>
    cv_status wait_for(Lock& lock, const std::chrono::duration<Rep, Period>& rel_time);

    template<typename Lock, typename Rep, typename Period, typename Predicate>
    bool wait_for(Lock& lock, const std::chrono::duration<Rep, Period>& rel_time, Predicate pred);
};

#endif //MTHREAD_MCONDITION_VARIABLE_H
