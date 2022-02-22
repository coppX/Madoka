//
// Created by 最上川 on 2021/12/11.
//

#ifndef MTHREAD_MFUTURE_H
#define MTHREAD_MFUTURE_H
#include <chrono>
#include <type_traits.h>
namespace M
{
    template<typename T>
    class future;

    template<typename T>
    class shared_future;

    enum class future_status {
        ready,
        timeout,
        deferred
    };

    class exception_ptr;
    
    struct allocator_arg_t { explicit allocator_arg_t() = default; };

    template<typename R>
    class promise
    {
    public:
        promise();
        template<typename Allocator>
        promise(allocator_arg_t, const Allocator& a);
        promise(promise&& rhs) noexcept;
        promise& operator=(promise&& rhs) noexcept;

        promise(const promise&) = delete;
        promise& operator=(const promise&) = delete;

        ~promise();
        void swap(promise& other) noexcept;

        future<R> get_future();

        void set_value(const R& r);
        void set_value(R&& r);
        void set_exception(exception_ptr p);

        void set_value_at_thread_exit(const R& r);
        void set_value_at_thread_exit(R&& r);
        void set_exception_at_thread_exit(exception_ptr p);
    };

    template<typename T>
    class future
    {
    public:
        future() noexcept;
        ~future();

        future(future&& other) noexcept;
        future& operator=(future&& other) noexcept;

        future(const future& other) = delete;
        future& operator=(const future& other) = delete;

        shared_future<T> shared();

        T get();

        bool valid();
        void wait() const;

        template<typename Rep, typename Period>
        future_status wait_for(const std::chrono::duration<Rep, Period>& timeout_duration) const;

        template<typename Clock, typename Duration>
        future_status wait_until(const std::chrono::time_point<Clock, Duration>& timeout_time) const;
    };

    template<typename Function, typename... Args>
    future<typename result_of<typename decay<Function>::type(typename decay<Args>::type...)>::type>
            async(Function&& f, Args&&... args);

}
#endif //MTHREAD_MFUTURE_H
