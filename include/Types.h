#ifndef __TYPES__
#define __TYPES__

#if defined (__APPLE__) || defined (__linux__)
#include <pthread.h>
#include <sys/sysctl.h>
#elif defined (_WIN32)
#include <process.h>
#include <xthreads.h>
#include <windows.h>
#include <synchapi.h>
#endif

#if defined (__clang__) || defined (__GNUC__)
#define CPP_STANDARD __cplusplus	
#elif defined (_MSC_VER)
#define CPP_STANDARD _MSVC_LANG
#endif

#if CPP_STANDARD < 201103L
#define _LIBCPP_CXX03_LANG
#elif CPP_STANDARD < 201402L
#define _LIBCPP_CXX11_LANG
#elif CPP_STANDARD < 201703L
#define _LIBCPP_CXX14_LANG
#elif CPP_STANDARD == 201703L
#define _LIBCPP_CXX17_LANG
#else
#define _LIBCPP_CXX20_LANG
#endif

#ifdef _LIBCPP_CXX20_LANG
#include <compare>
#endif


#endif /* Types_h */
