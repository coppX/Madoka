//
// Created by FDC on 2021/12/8.
//

#ifndef MTHREAD_THREAD_IMPL_H
#define MTHREAD_THREAD_IMPL_H

#include <ostream>

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
#define MUTEXT_INITIALIZER PTHREAD_MUTEX_INITIALIZER
#elif defined (WINDOWS)
#define MUTEXT_INITIALIZER nullptr
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
    return pthread_mutex_trylock(m);
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

#endif //MTHREAD_THREAD_IMPL_H