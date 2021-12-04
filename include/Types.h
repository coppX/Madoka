//
//  Types.h
//  test
//
//  Created by 最上川 on 2021/12/4.
//

#ifndef __TYPES__
#define __TYPES__
#include <tuple>

#if defined (__APPLE__) || defined (__linux__)
typedef pthread_t thread_t;
#elif defined (_WIN32)
typedef unsigned int thread_t;
#endif



class ThreadId{
public:
    thread_t id_;
    ThreadId() : id_(0) {}
    ThreadId(thread_t id) : id_(id) {}
    friend bool operator==(ThreadId x, ThreadId y)
    {
        return x.id_ == y.id_;
    }
    friend bool operator!=(ThreadId x, ThreadId y)
    {
        return x.id_ != y.id_;
    }
    friend bool operator<(ThreadId x, ThreadId y)
    {
        return x.id_ < y.id_;
    }
    friend bool operator<=(ThreadId x, ThreadId y)
    {
        return x.id_ <= y.id_;
    }
    friend bool operator>(ThreadId x, ThreadId y)
    {
        return x.id_ > y.id_;
    }
    friend bool operator>=(ThreadId x, ThreadId y)
    {
        return x.id_ >= y.id_;
    }
    
    
    friend class MThread;
};

#if __cplusplus < 201103L
#define _LIBCPP_CXX03_LANG
#elif __cplusplus < 201402L
#define _LIBCPP_CXX11_LANG
#elif __cplusplus < 201703L
#define _LIBCPP_CXX14_LANG
#endif

// version < C++14
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

#endif /* Types_h */
