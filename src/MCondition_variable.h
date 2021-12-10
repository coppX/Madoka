//
// Created by FDC on 2021/12/7.
//

#ifndef MTHREAD_MCONDITION_VARIABLE_H
#define MTHREAD_MCONDITION_VARIABLE_H
#include "Types.h"
#include "Mutex.h"
using namespace std::chrono;

namespace M {

    enum class cv_status {
        no_timeout, timeout
    };

    class condition_variable {
    public:
        condition_variable() = default;

        ~condition_variable() {
            condition_variable_destroy(&cv_);
        }

        condition_variable(condition_variable &) = delete;

        condition_variable &operator=(condition_variable &) = delete;

        void notify_all() noexcept {
            condition_variable_boardcast(&cv_);
        }

        void notify_one() noexcept {
            condition_variable_signal(&cv_);
        }

        void wait(unique_lock<mutex> &lock) {
            condition_variable_wait(&cv_, lock.mutex()->native_handle());
        }

        template<typename Predicate>
        void wait(unique_lock<mutex> &lock, Predicate pred) {
            while (!pred())
                wait(lock);
        }

        template<typename Clock, typename Duration>
        cv_status wait_until(unique_lock<mutex> &lock, const time_point<Clock, Duration> &abs_time) {
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
        wait_until(unique_lock<mutex> &lock, const time_point<Clock, Duration> &abs_time, Predicate pred) {
            while (!pred()) {
                if (wait_until(lock, abs_time) == M::cv_status::timeout) {
                    return pred();
                }
            }
            return true;
        }

        template<typename Rep, typename Period>
        cv_status wait_for(unique_lock<mutex> &lock, const duration<Rep, Period> &rel_time) {
            if (rel_time < rel_time.zero())
                return M::cv_status::timeout;
            using ns_rep = nanoseconds::rep;
            steady_clock::time_point now = steady_clock::now();
            using clock_tp_ns = time_point<system_clock, nanoseconds>;
            ns_rep now_count_ns = now.time_since_epoch().count();
            ns_rep d_ns_count = rel_time.count();

            if (now_count_ns > std::numeric_limits<ns_rep>::max() - d_ns_count) {
                _do_timed_wait(lock, clock_tp_ns::max());
            } else {
                _do_timed_wait(lock, clock_tp_ns(nanoseconds(now_count_ns + d_ns_count)));
            }
            return steady_clock::now() - now < rel_time ? M::cv_status::no_timeout : M::cv_status::timeout;
        }

        template<typename Rep, typename Period, typename Predicate>
        bool wait_for(unique_lock<mutex> &lock, const duration<Rep, Period> &rel_time, Predicate pred)
        {
            return wait_until(lock, steady_clock::now() + rel_time, pred);
        }

        typedef cond_t* native_handle_type;
        native_handle_type native_handle() { return &cv_; }
    private:

        void _do_timed_wait(unique_lock<mutex>& lk, time_point<system_clock, nanoseconds> tp)
        {
            if (!lk.owns_lock())
            {
                printf("condition_variable time_wait: mutex not locked");
            }
            nanoseconds d = tp.time_since_epoch();
            timespec_t ts;
            seconds s = duration_cast<seconds>(d);
            ts.tv_sec = static_cast<decltype(ts.tv_sec)>(s.count());
            ts.tv_nsec = static_cast<decltype(ts.tv_nsec)>((d - s).count());
            int ec = condition_variable_timedwait(&cv_, lk.mutex()->native_handle(), &ts);
            if (ec && ec != ETIMEDOUT)
            {
                printf("condition_variable timed_wait failed");
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

};
#endif //MTHREAD_MCONDITION_VARIABLE_H
