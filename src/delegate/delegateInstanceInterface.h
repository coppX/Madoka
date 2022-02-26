//
// Created by 最上川 on 2022/2/26.
//

#ifndef MADOKA_DELEGATEINSTANCEINTERFACE_H
#define MADOKA_DELEGATEINSTANCEINTERFACE_H

#include <tuple>
#include "allocator.h"

namespace M
{
    class fDelegateBase;
    class iDelegateInstance;

    template<typename funcType, typename userPolicy>
    struct iBaseDelegateInstance;

    template<typename retType, typename... argTypes, typename userPolicy>
    struct iBaseDelegateInstance<retType(argTypes...), userPolicy>
            : public userPolicy::fDelegateInstanceExtras
    {
        virtual void createCopy(fDelegateBase& base) = 0;
        virtual retType execute(argTypes...) const = 0;
        virtual bool executeIfSafe(argTypes...) const = 0;
    };

    template<bool isConst, typename classType, typename funcType>
    struct tMemFuncPtrType;

    template<typename classType, typename retType, typename... argTypes>
    struct tMemFuncPtrType<false, classType, retType(argTypes...)>
    {
        typedef retType (classType::* type)(argTypes...);
    };

    template<typename classType, typename retType, typename... argTypes>
    struct tMemFuncPtrType<true, classType, retType(argTypes...)>
    {
        typedef retType (classType::* type)(argTypes...) const;
    };

    template<typename funcType>
    struct tPayload;

    template<typename ret, typename... types>
    struct tPayload<ret(types...)>
    {
        std::tuple<types..., ret> values;

        template<typename... argTypes>
        explicit tPayload(argTypes&&... args)
            : values(std::forward<argTypes>(args)..., ret())
        {}

        ret& getResult()
        {
            return std::get<sizeof...(types)>(values);
        }
    };

    template<typename... types>
    struct tPayload<void(types...)>
    {
        std::tuple<types...> values;

        template<typename... argTypes>
        explicit tPayload(argTypes&&... args)
            : values(std::forward<argTypes>(args)...)
        {}

        void getResult()
        {}
    };

    template<typename T>
    struct tPlacementNewer
    {
        tPlacementNewer()
            : bConstructed(false)
        {}

        ~tPlacementNewer()
        {
            if (bConstructed)
            {
                ((T*)&bytes)->T::~T();
            }
        }

        template<typename... argsTypes>
        T* operator()(argsTypes&&... args)
        {
            assert(!bConstructed);
            T* result = new (&bytes) T(std::forward<argsTypes>(args)...);
            bConstructed = true;
            return result;
        }
        T* operator->()
        {
            return (T*)&bytes;
        }
    private:
        tTypeCompatibleBytes<T> bytes;
        bool                    bConstructed;
    };
}

#endif //MADOKA_DELEGATEINSTANCEINTERFACE_H
