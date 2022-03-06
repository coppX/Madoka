//
// Created by 最上川 on 2022/3/6.
//

#ifndef MADOKA_FUNCTOR_H
#define MADOKA_FUNCTOR_H
#include <functorHelp.h>

namespace M
{
    template<typename T>
    struct Functor;

    template<typename R, typename... P>
    struct Functor<R(*)(P...)>
    {
        using BaseFunType = R(*)(P...);
        using BaseHelpPointer = IBaseFunctorHelp<R, P...>*;
        Functor() : fun(nullptr) {};
        ~Functor() { if (fun) delete fun; }

        Functor(const Functor&) = delete;
        Functor(Functor&&) = delete;
        Functor& operator=(const Functor&) = delete;
        Functor& operator=(Functor&&) = delete;

        template<typename F>
        Functor(F&& f)
            : fun(nullptr)
        {
            using FunType = std::decay_t<F>;
            using FunctorHelpType = FunctorHelp<FunType, R, P...>;
            fun = new FunctorHelpType(std::forward<F>(f));
        }

        R operator()(P... param) const
        {
            return fun->call(std::forward<P>(param)...);
        }

        explicit operator bool() const
        {
            return fun == nullptr;
        }
        friend bool operator==(const Functor& left, const Functor& right)
        {
            return left.fun == right.fun;
        }
    private:
        BaseHelpPointer  fun;
    };
}
#endif //MADOKA_FUNCTOR_H