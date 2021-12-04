#ifndef __TYPES__
#define __TYPES__

typedef unsigned long long ThreadId;
typedef void* thread_t;

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

// version < C++17
#ifndef _LIBCPP_CXX03_LANG && _LIBCPP_CXX11_LANG && _LIBCPP_CXX14_LANG
template<typename T>
using tuple_size_v = typename tuple_size<T>::value;
#endif

#endif