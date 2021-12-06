#ifndef __MTHREAD__
#define __MTHREAD__

#include <tuple>
#include <memory>
#include <cassert>
#include <functional>
#include "Types.h"

#ifndef _LIBCPP_CXX20_LANG
// remove const volatile and reference attribute
template<typename T >
struct remove_cvref {
    typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};

template<typename T>
using remove_cvref_t = typename remove_cvref<T>::type;
#endif

template<size_t...> struct tuple_indices {};

template<size_t Sp, class IntTuple, size_t Ep> struct make_indices_imp;

template<size_t Sp, size_t... Indices, size_t Ep>
struct make_indices_imp<Sp, tuple_indices<Indices...>, Ep>
{
    typedef typename make_indices_imp<Sp + 1, tuple_indices<Indices..., Sp>, Ep>::type type;
};

//recursion end Sp=Ep
template<size_t Ep, size_t... Indices>
struct make_indices_imp<Ep, tuple_indices<Indices...>, Ep>
{
    typedef tuple_indices<Indices...> type;
};

//indices
template<size_t Ep, size_t Sp = 0>
struct make_tuple_indices {
    typedef typename make_indices_imp<Sp, tuple_indices<>, Ep>::type type;
};

template<typename Tuple>
inline
#if defined (__APPLE__) || defined (__linux__)
void*
#elif defined (_WIN32)
unsigned int
#endif
Invoke(void* rawVals)
{
    std::unique_ptr<Tuple> tp(static_cast<Tuple*>(rawVals));
    typedef typename make_tuple_indices<std::tuple_size_v<Tuple>, 1>::type IndexType;
    thread_execute(*tp, IndexType());
#if defined (__APPLE__) || defined (__linux__)
    return nullptr;
#elif defined (_WIN32)
    return 0;
#endif
}

template<typename Tuple, size_t... Indices>
inline void thread_execute(Tuple& tp, tuple_indices<Indices...>)
{
    std::invoke(std::move(std::get<0>(tp)), std::move(std::get<Indices>(tp))...);
}

class MThread
{
public:
    //delete copy constructor/operator
    MThread(const MThread&) = delete;
    MThread& operator=(const MThread&) = delete;

    //typedef
#if defined (__APPLE__) || defined (__linux__)
    typedef int native_handle_type;
#elif defined (_WIN32)
    typedef void* native_handle_type;
#endif

    //constructor
    MThread() : t_{} {}
    template<typename F, typename... Args,
        typename = std::enable_if_t<!std::is_same_v<remove_cvref_t<F>, MThread>>>
        MThread(F&& f, Args&&... args)
    {
        typedef std::tuple<std::decay_t<F>, std::decay_t<Args>...> Tp;
        std::unique_ptr<Tp> tp(new Tp(std::forward<F>(f), std::forward<Args>(args)...));

        int ec = -1;
#if defined (__APPLE__) || defined (__linux__)
        t_._Hnd = pthread_create(&t_._Id, nullptr, &Invoke<Tp>, tp.get());
#elif defined (_WIN32)
        t_._Hnd = reinterpret_cast<void*>(_beginthreadex(nullptr, 0, &Invoke<Tp>, tp.get(), 0, &t_._Id));
#endif
        if (t_._Id)
        {
            tp.release();
        }
        else
        {
            t_._Id = 0;
            printf("thread constructor failed");
            std::abort();
        }
    }

    ~MThread()
    {
        assert(!joinable());
    }
    MThread(MThread&& t)
        : t_(t.t_)
    {
        t.t_ = {};
    }

    MThread& operator=(MThread&& t)
    {
        t_ = t.t_;
        t.t_ = {};
        return *this;
    }

    //observers
    bool joinable() const noexcept
    {
        return 0 != t_._Id;
    }

    ThreadId get_id() const noexcept
    {
        return t_._Id;
    }

    native_handle_type native_handle()
    {
        return t_._Hnd;
    }

    static unsigned int hardware_concurrency() noexcept
    {
#if defined (__APPLE__) || defined (__linux__)
        unsigned n;
        int mib[2] = { CTL_HW, HW_NCPU };
        std::size_t s = sizeof(n);
        sysctl(mib, 2, &n, &s, 0, 0);
        return n;
#elif defined (_WIN32)
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        return info.dwNumberOfProcessors;
#endif
    }

    //operations
    void join()
    {
        if (0 != t_._Id)
        {
            int ec = -1;
#if defined (__APPLE__) || defined (__linux__)
            ec = pthread_join(t_._Id, 0);
#elif defined (_WIN32)
            ec = _Thrd_join(t_, nullptr);
#endif
            if (0 != ec)
            {
                printf("thread join failed");
                std::abort();
            }
            t_ = {};
        }
    }
    void detach()
    {
        if (0 != t_._Id)
        {
            int ec = -1;
#if defined (__APPLE__) || defined (__linux__)
            ec = pthread_detach(t_._Id);
#elif defined (_WIN32)
            ec = _Thrd_detach(t_);
#endif
            if (0 != ec)
            {
                printf("thread detach failed");
                std::abort();
            }
            t_ = {};
        }
    }
    void swap(MThread& t) noexcept
    {
        std::swap(t.t_, t_);
    }

private:
    ThreadId threadId_;
    thread_t t_;
};

#endif /* MThread_h */
