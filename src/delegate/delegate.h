//
// Created by 最上川 on 2022/2/25.
//

#ifndef MADOKA_DELEGATE_H
#define MADOKA_DELEGATE_H

namespace M
{
    template<typename DelegateSignature, typename UserPolicy>
    class delegate
    {
        static_assert(sizeof(DelegateSignature) == 0, "Expected a function signature for the delegate template parameter");
    };

    template<typename RetType, typename... Params, typename UserPolicy>
    class delegate<RetType(Params...), UserPolicy>
    {

    };

    template<typename DelegateSignature, typename UserPolicy>
    class multicastDelegate
    {
        static_assert(sizeof(DelegateSignature) == 0, "Expected a function signature for the delegate template parameter");
    };

    template<typename RetType, typename... Params, typename UserPolicy>
    class multicastDelegate<RetType(Params...), UserPolicy>
    {

    };

#define FUNC_DECLARE_DELEGATE(delegateName, returnType, ...) \
    typedef delegate<retureType(__VA_ARGS__)> delegateName;

#define FUNC_DECLARE_MULTICAST_DELEGATE(multicastDelegateName, returnType, ...) \
    typedef multicastDelegate<returnType(__VA_ARGS__)> multicastDelegateName;

#define DECLARE_DELEGATE(delegateName) FUNC_DECLARE_DELEGATE(delegateName void)
#define DECLARE_MULTICAST_DELEGATE(delegateName) FUNC_DECLARE_MULTICAST_DELEGATE(delegateName, void)
}
#endif //MADOKA_DELEGATE_H
