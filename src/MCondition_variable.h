//
// Created by FDC on 2021/12/7.
//

#ifndef MTHREAD_MCONDITION_VARIABLE_H
#define MTHREAD_MCONDITION_VARIABLE_H
#include "Types.h"
#include "Mutex.h"
namespace M {

    enum class cv_status {
        no_timeout, timeout
    };

    class condition_variable {
    public:
        condition_variable();

        ~condition_variable();

        condition_variable(condition_variable &) = delete;

        condition_variable &operator=(condition_variable &) = delete;

        void notify_all() noexcept;

        void notify_one() noexcept;

        void wait(unique_lock<mutex> &lock);

        template<typename Predicate>
        void wait(unique_lock<mutex> &lock, Predicate pred);

        template<typename Clock, typename Duration>
        cv_status wait_until(unique_lock<mutex> &lock, const std::chrono::time_point<Clock, Duration> &abs_time);

        template<typename Clock, typename Duration, typename Predicate>
        bool
        wait_util(unique_lock<mutex> &lock, const std::chrono::time_point<Clock, Duration> &abs_time, Predicate pred);

        template<typename Rep, typename Period>
        cv_status wait_for(unique_lock<mutex> &lock, const std::chrono::duration<Rep, Period> &rel_time);

        template<typename Rep, typename Period, typename Predicate>
        bool wait_for(unique_lock<mutex> &lock, const std::chrono::duration<Rep, Period> &rel_time, Predicate pred);

#if defined (__APPLE__) || defined (__linux__)
        typedef pthread_cond_t* native_handle_type;
#elif defined (_WIN32)
        typedef void *native_handle_type;
#endif

        native_handle_type native_handle();
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
