#ifndef __MTHREAD__
#define __MTHREAD__

#include <tuple>
#include <memory>
#include <cassert>
#include <functional>
#include "thread_header.h"
#include "thread_helper.h"

namespace M {

// C++ version < C++20
#ifndef _LIBCPP_CXX20_LANG
// remove const volatile and reference attribute
    template<typename T>
    struct remove_cvref {
        typedef std::remove_cv_t<std::remove_reference_t<T>> type;
    };

    template<typename T>
    using remove_cvref_t = typename remove_cvref<T>::type;
#endif

// this struct defined is to dedution a indices
    template<size_t...>
    struct tuple_indices {
    };

    template<size_t Sp, typename IntTuple, size_t Ep>
    struct make_indices_imp;

    template<size_t Sp, size_t... Indices, size_t Ep>
    struct make_indices_imp<Sp, tuple_indices<Indices...>, Ep> {
        // every recursion, the Indices will be 1, 2, 3... + Sp, and Sp += 1
        typedef typename make_indices_imp<Sp + 1, tuple_indices<Indices..., Sp>, Ep>::type type;
    };

// the recursion end, Sp=Ep
    template<size_t Ep, size_t... Indices>
    struct make_indices_imp<Ep, tuple_indices<Indices...>, Ep> {
        typedef tuple_indices<Indices...> type;
    };

//indices
    template<size_t Ep, size_t Sp = 0>
    struct make_tuple_indices {
        // the tuple_indices<> here is to store the sequence [1, tuple_size) when template dedutions
        typedef typename make_indices_imp<Sp, tuple_indices<>, Ep>::type type;
    };

    template<typename Tuple>
    inline
#if defined (POSIX)
    void *
#elif defined (WINDOWS)
    unsigned int
#endif
    Invoke(void *rawVals) {
        std::unique_ptr<Tuple> tp(static_cast<Tuple *>(rawVals));
        typedef typename make_tuple_indices<std::tuple_size_v<Tuple>, 1>::type IndexType;
        // IndexType will be tuple_indices<1, 2, 3, 4 ... tuple_size_v<Tuple> - 1>
        thread_execute(*tp, IndexType());
#if defined (POSIX)
        return nullptr;
#elif defined (WINDOWS)
        return 0;
#endif
    }

    template<typename Tuple, size_t... Indices>
    inline void thread_execute(Tuple &tp, tuple_indices<Indices...>) {
        // then dedution Indices will be 1, 2, 3 ... uple_size_v<Tuple> - 1
        std::invoke(std::move(std::get<0>(tp)), std::move(std::get<Indices>(tp))...);
    }

    thread::thread() noexcept: t_{} {}

    template<typename F, typename... Args,
            typename = std::enable_if_t<!std::is_same_v<remove_cvref_t<F>, thread>>>
    thread::thread(F &&f, Args &&... args) {
        typedef std::tuple<std::decay_t<F>, std::decay_t<Args>...> Tp;
        std::unique_ptr<Tp> tp(new Tp(std::forward<F>(f), std::forward<Args>(args)...));

        t_ = {};
        thread_create(&t_, &Invoke<Tp>, tp.get());
        if (t_._Id) {
            tp.release();
        } else {
            t_._Id = 0;
            printf("thread constructor failed\n");
            std::abort();
        }
    }

    thread::~thread() {
        assert(!joinable());
    }

    thread::thread(thread &&t) noexcept
        : t_(t.t_)
    {
        t.t_ = {};
    }

    thread &thread::operator=(thread &&t) noexcept
    {
        t_ = t.t_;
        t.t_ = {};
        return *this;
    }

    bool thread::joinable() const noexcept
    {
        return 0 != t_._Id;
    }

    ThreadId thread::get_id() const noexcept {
        return t_._Id;
    }

    thread::native_handle_type thread::native_handle()
    {
        return t_._Hnd;
    }

    unsigned int thread::hardware_concurrency() noexcept {
        return hw_concurrency();
    }

    void thread::join() {
        if (joinable()) {
            if (0 != thread_join(&t_)) {
                printf("thread join failed\n");
                std::abort();
            }
            t_ = {};
        }
    }

    void thread::detach() {
        if (joinable()) {
            if (0 != thread_detach(&t_)) {
                printf("thread detach failed\n");
                std::abort();
            }
            t_ = {};
        }
    }

    void thread::swap(thread &t) noexcept
    {
        std::swap(t.t_, t_);
    }
};

#endif /* MThread_h */
