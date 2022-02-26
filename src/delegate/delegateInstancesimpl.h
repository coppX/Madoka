//
// Created by 最上川 on 2022/2/26.
//

#ifndef MADOKA_DELEGATEINSTANCESIMPL_H
#define MADOKA_DELEGATEINSTANCESIMPL_H

#include <tuple>
#include "delegateInstanceInterface.h"
#include "delegateInstance.h"
namespace M
{
    class fDelegateBase;
    class fDelegateHandle;

    template<typename funcType, typename userPolicy, typename... varTypes>
    class tCommonDelegateInstanceState;

    template<typename retType, typename... paramTypes, typename userPolicy, typename... varTypes>
    class tCommonDelegateInstanceState<retType(paramTypes...), userPolicy, varTypes...>
            : iBaseDelegateInstance<retType(paramTypes...), userPolicy>
    {
    public:
        explicit tCommonDelegateInstanceState(varTypes... vars)
            : payload(vars...)
            , handle(fDelegateHandle::newHandle)
        {}

        fDelegateHandle getHandle() const final
        {
            return handle;
        }
    private:
        std::tuple<varTypes...> payload;
        fDelegateHandle         handle;
    };
}

#endif //MADOKA_DELEGATEINSTANCESIMPL_H
