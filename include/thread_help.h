//
// Created by FDC on 2021/12/8.
//

#ifndef MTHREAD_THREAD_IMPL_H
#define MTHREAD_THREAD_IMPL_H

#include <ostream>
#include <chrono>

#if defined (__APPLE__) || defined (__linux__)
#define POSIX
#elif defined (_WIN32)
#define WINDOWS
#endif

#if defined (POSIX)
typedef pthread_t _Thrd_id_t;
struct thread_t {
    _Thrd_id_t _Id;
    int _Hnd;
};
#elif defined (WINDOWS)
typedef _Thrd_t thread_t;
#endif

class ThreadId {
public:
    _Thrd_id_t id_;
    ThreadId() : id_(0) {}
    ThreadId(_Thrd_id_t id) : id_(id) {}

#ifdef _LIBCPP_CXX20_LANG
    friend std::strong_ordering operator<=>(ThreadId& x, ThreadId& y)
    {
        return x.id_ <=> y.id_;
    }
#else
    friend bool operator==(ThreadId& x, ThreadId& y)
    {
        return x.id_ == y.id_;
    }
    friend bool operator!=(ThreadId& x, ThreadId& y)
    {
        return x.id_ != y.id_;
    }
    friend bool operator<(ThreadId& x, ThreadId& y)
    {
        return x.id_ < y.id_;
    }
    friend bool operator<=(ThreadId& x, ThreadId& y)
    {
        return x.id_ <= y.id_;
    }
    friend bool operator>(ThreadId& x, ThreadId& y)
    {
        return x.id_ > y.id_;
    }
    friend bool operator>=(ThreadId& x, ThreadId& y)
    {
        return x.id_ >= y.id_;
    }
#endif

    template<typename Ch, typename Tr>
    friend std::basic_ostream<Ch, Tr>& operator<<(std::basic_ostream<Ch, Tr>& os, ThreadId id)
    {
        return os << id.id_;
    }

    friend class MThread;
};

// C++ version < C++14
#ifdef _LIBCPP_CXX11_LANG
template<typename T>
using decay_t = typename decay<T>::type;

template<typename T>
using is_same_t = typename is_same<T>::type;

template<typename T>
using is_same_v = typename is_same<T>::value;

template<typename T>
using enable_if_t = typename enable_if<T>::type;

template<typename T>
using remove_cv_t = typename remove_cv<T>::type;

template<typename T>
using remove_reference_t = typename remove_reference<T>::type;
#endif

//********************MUTEX*************************

#if defined (POSIX)
#define MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER
#define COND_INITIALIZER PTHREAD_COND_INITIALIZER
#elif defined (WINDOWS)
#define MUTEX_INITIALIZER nullptr
#define COND_INITIALIZER nullptr
#endif

#if defined (POSIX)
typedef pthread_mutex_t mutex_t;
#elif defined (WINDOWS)
typedef void *mutex_t;
#endif

int mutex_lock(mutex_t* m)
{
#if defined (POSIX)
    return pthread_mutex_lock(m);
#elif defined (WINDOWS)
    AcquireSRWLockExclusive((PSRWLOCK)m);
    return 0;
#endif
}

bool mutex_trylock(mutex_t* m)
{
#if defined (POSIX)
    return 0 == pthread_mutex_trylock(m);
#elif defined (WINDOWS)
    return TryAcquireSRWLockExclusive((PSRWLOCK)m);
#endif
}

int mutex_unlock(mutex_t* m)
{
#if defined (POSIX)
    return pthread_mutex_unlock(m);
#elif defined (WINDOWS)
    ReleaseSRWLockExclusive((PSRWLOCK)m);
    return 0;
#endif
}

int mutex_destroy(mutex_t* m)
{
#if defined (POSIX)
    return pthread_mutex_destroy(m);
#elif defined (WINDOWS)
    return 0;
#endif
}
//*********************************************

//*******************recursive_mutex*****************
#if defined (POSIX)
typedef pthread_mutex_t recursive_mutex_t;
#elif defined (WINDOWS)
typedef void* recursive_mutex_t[6];
#endif
int recursive_mutex_init(recursive_mutex_t* m)
{
#if defined (POSIX)
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    return pthread_mutex_init(m, &attr);
#elif defined (WINDOWS)
    InitializeCriticalSection((LPCRITICAL_SECTION)m);
    return 0;
#endif
}

int recursive_mutex_lock(recursive_mutex_t* m)
{
#if defined (POSIX)
    return pthread_mutex_lock(m);
#elif defined (WINDOWS)
    EnterCriticalSection((LPCRITICAL_SECTION)m);
    return 0;
#endif
}

bool recursive_mutex_trylock(recursive_mutex_t* m)
{
#if defined (POSIX)
    return pthread_mutex_trylock(m);
#elif defined (WINDOWS)
    return TryEnterCriticalSection((LPCRITICAL_SECTION)m);
#endif
}

int recursive_mutex_unlock(recursive_mutex_t* m)
{
#if defined (POSIX)
    return pthread_mutex_unlock(m);
#elif defined (WINDOWS)
    LeaveCriticalSection((LPCRITICAL_SECTION)m);
    return 0;
#endif
}

int recursive_mutex_destroy(recursive_mutex_t* m)
{
#if defined (POSIX)
    return pthread_mutex_destroy(m);
#elif defined (WINDOWS)
    DeleteCriticalSection((LPCRITICAL_SECTION)m);
    return 0;
#endif
}

//***************************************************

//****************condition_variable*****************
#if defined (POSIX)
typedef pthread_cond_t cond_t;
#elif defined (WINDOWS)
typedef void* cond_t;
#endif
typedef ::timespec timespec_t;

int condition_variable_signal(cond_t* cv)
{
#if defined (POSIX)
    return pthread_cond_signal(cv);
#elif defined (WINDOWS)
    WakeConditionVariable((PCONDITION_VARIABLE)cv);
    return 0;
#endif
}

int condition_variable_boardcast(cond_t* cv)
{
#if defined (POSIX)
    return pthread_cond_boardcast(cv);
#elif defined (WINDOWS)
    WakeAllConditionVariable((PCONDITION_VARIABLE)cv);
    return 0;
#endif
}

int condition_variable_wait(cond_t* cv, mutex_t* m)
{
#if defined (POSIX)
    return pthread_cond_wait(cv, m);
#elif defined (WINDOWS)
    SleepConditionVariableSRW((PCONDITION_VARIABLE)cv, (PSRWLOCK)m, INFINITE, 0);
    return 0;
#endif
}

int condition_variable_timedwait(cond_t* cv, mutex_t* m, timespec_t* t)
{
#if defined (POSIX)
    return pthread_cond_timedwait(cv, m, t);
#elif defined (WINDOWS)
    using namespace std::chrono;
    auto duration = seconds(t->tv_sec) + nanoseconds(t->tv_nsec);
    auto abstime = system_clock::time_point(duration_cast<system_clock::duration>(duration));
    auto timeout_ms = duration_cast<milliseconds>(abstime - system_clock::now());

    if (!SleepConditionVariableSRW((PCONDITION_VARIABLE)cv, (PSRWLOCK)m, timeout_ms.count() > 0 ? timeout_ms.count() : 0, 0))
    {
        printf("time out");
        return ETIMEDOUT;
    }
#endif
}

int condition_variable_destroy(cond_t* cv)
{
#if defined (POSIX)
    return pthread_cond_destroy(cv);
#elif defined (WINDOWS)
    return 0;
#endif
}

//***************************************************
#endif //MTHREAD_THREAD_IMPL_H
