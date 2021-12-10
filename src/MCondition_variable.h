//
// Created by FDC on 2021/12/7.
//

#ifndef MTHREAD_MCONDITION_VARIABLE_H
#define MTHREAD_MCONDITION_VARIABLE_H
#include "Types.h"
#include "thread_header.h"

namespace M {

    condition_variable::~condition_variable()
    {
        condition_variable_destroy(&cv_);
    }

    void condition_variable::notify_all() noexcept
    {
        condition_variable_boardcast(&cv_);
    }

    void condition_variable::notify_one() noexcept {
        condition_variable_signal(&cv_);
    }

    void condition_variable::wait(unique_lock<mutex> &lock)
    {
        condition_variable_wait(&cv_, lock.mutex()->native_handle());
    }

    template<typename Predicate>
    void condition_variable::wait(unique_lock<mutex> &lock, Predicate pred)
    {
        while (!pred())
            wait(lock);
    }

    template<typename Clock, typename Duration>
    cv_status
    condition_variable::wait_until(unique_lock<mutex> &lock, const time_point<Clock, Duration> &abs_time) {
        using clock_tp_ns = time_point<Clock, nanoseconds>;
        typename Clock::time_point _now = Clock::now();
        if (abs_time <= _now)
            return M::cv_status::timeout;

        clock_tp_ns t_ns = clock_tp_ns(abs_time.time_since_epoch());
        _do_timed_wait(lock, t_ns);
        return Clock::now() < abs_time ? M::cv_status::no_timeout : M::cv_status::timeout;
    }

    template<typename Clock, typename Duration, typename Predicate>
    bool
    condition_variable::wait_until(unique_lock<mutex> &lock, const time_point<Clock, Duration> &abs_time, Predicate pred) {
        while (!pred()) {
            if (wait_until(lock, abs_time) == M::cv_status::timeout) {
                return pred();
            }
        }
        return true;
    }

    template<typename Rep, typename Period>
    cv_status
    condition_variable::wait_for(unique_lock<mutex> &lock, const duration<Rep, Period> &rel_time) {
        if (rel_time < rel_time.zero())
            return M::cv_status::timeout;
        using ns_rep = nanoseconds::rep;
        steady_clock::time_point now = steady_clock::now();
        using clock_tp_ns = time_point<system_clock, nanoseconds>;
        ns_rep now_count_ns = now.time_since_epoch().count();
        ns_rep d_ns_count = rel_time.count();

        if (now_count_ns > (std::numeric_limits<ns_rep>::max)() - d_ns_count) {
            _do_timed_wait(lock, (clock_tp_ns::max)());
        } else {
            _do_timed_wait(lock, clock_tp_ns(nanoseconds(now_count_ns + d_ns_count)));
        }
        return steady_clock::now() - now < rel_time ? M::cv_status::no_timeout : M::cv_status::timeout;
    }

    template<typename Rep, typename Period, typename Predicate>
    bool
    condition_variable::wait_for(unique_lock<mutex> &lock, const duration<Rep, Period> &rel_time, Predicate pred)
    {
        return wait_until(lock, steady_clock::now() + rel_time, std::move(pred));
    }

};
#endif //MTHREAD_MCONDITION_VARIABLE_H
