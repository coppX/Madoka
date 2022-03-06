//
// Created by 最上川 on 2022/3/6.
//

#ifndef MADOKA_FUNCTORHELP_H
#define MADOKA_FUNCTORHELP_H
#include <utility>
#include <type_traits>

namespace M
{
    template<typename R, typename ...P>
    struct IBaseFunctorHelp
    {
        virtual ~IBaseFunctorHelp() = default;
        virtual R call(P... param) const = 0;
        virtual bool equal(const IBaseFunctorHelp* other) const = 0;
    };

    template<typename T, typename = void>
    struct AutoTypeEqualSimple
    {
        static bool equal(const T& left, const T& right)
        {
            return false;
        }
    };

    template<typename T>
    struct AutoTypeEqualSimple<T, std::void_t<decltype(std::declval<const T&>() == std::declval<const T&>())>>
    {
        static bool equal(const T& left, const T& right)
        {
            return left == right;
        }
    };

    template<typename F, typename R, typename ...P>
    struct FunctorHelp : public IBaseFunctorHelp<R, P...>
    {
        using FunType = F;
        using SelfType = FunctorHelp<F, R, P...>;
        using BaseType = IBaseFunctorHelp<R, P...>;

        template<typename U>
        FunctorHelp(U&& f) : fun(std::forward<U>(f)) {}

        virtual R call(P... param) const override
        {
            return fun(std::forward<P>(param)...);
        }

        virtual bool equal(const BaseType* other)  const override
        {
            const SelfType* functor = dynamic_cast<const SelfType*>(other);
            if (functor)
            {
                return AutoTypeEqualSimple<F>::equal(fun, functor->fun);
            }
            return false;
        }

    private:
        FunType fun;
    };
}
#endif //MADOKA_FUNCTORHELP_H