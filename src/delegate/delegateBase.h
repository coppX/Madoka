//
// Created by 最上川 on 2022/2/26.
//
#include "allocator.h"
#include "delegateInstance.h"
#ifndef MADOKA_DELEGATEBASE_H
#define MADOKA_DELEGATEBASE_H

namespace M
{
    template<typename T>
    static T DivideAndRoundUp(T divided, T divisor)
    {
        return (divided + divisor - 1) / divisor;
    }

    typedef tAlignBytes<16, 16> delegateType;
    class fDelegateBase
    {
    public:
        explicit fDelegateBase()
            : delegateSize(0)
        {}

        ~fDelegateBase()
        {
            unBind();
        }
        fDelegateBase(fDelegateBase&& other)
        {
            delegateAllocator.moveToEmpty(other.delegateAllocator);
            delegateSize = other.delegateSize;
            other.delegateSize = 0;
        }
        fDelegateBase& operator=(fDelegateBase&& other)
        {
            unBind();
            delegateAllocator.moveToEmpty(other.delegateAllocator);
            delegateSize = other.delegateSize;
            other.delegateSize = 0;
            return *this;
        }
        void unBind()
        {
            if (iDelegateInstance* instance = getDelegateInstanceProtected())
            {
                instance->~iDelegateInstance();
                delegateAllocator.resizeAllocation(0, 0, sizeof(delegateType));
                delegateSize = 0;
            }
        }

        iDelegateInstance* getDelegateInstanceProtected() const
        {
            return delegateSize ? (iDelegateInstance*)delegateAllocator.getAllocation() : nullptr;
        }

        void* allocate(int32_t size)
        {
            if (iDelegateInstance* instance = getDelegateInstanceProtected())
            {
                instance->~iDelegateInstance();
            }
            int32_t newDelegateSize = DivideAndRoundUp(size, (int32_t)sizeof(delegateType));
            if (newDelegateSize != delegateSize)
            {
                delegateAllocator.resizeAllocation(0, newDelegateSize, sizeof(delegateType));
                delegateSize = newDelegateSize;
            }
            return delegateAllocator.getAllocation();
        }
    private:
        int delegateSize;
        heapAllocator::forElementType<delegateType> delegateAllocator;
    };

    struct defaultDelegateUserPolicy
    {
        using fDelegateInstanceExtras    = iDelegateInstance;
        using fDelegateExtras            = fDelegateBase;
    };

    template<typename userPolicy>
    class tDelegateBase : public userPolicy::fDelegateExtras
    {
        using super = typename userPolicy::fDelegateExtras;
    public:
        string tryGetBoundFunctionName() const
        {
            if (iDelegateInstance* ptr = super::getDelegateInstanceProtected())
            {
                return ptr->tryGetBoundFunctionName();
            }
            return "";
        }

        void* getObject() const
        {
            if (iDelegateInstance* ptr = super::getDelegateInstanceProtected())
            {
                return ptr->getObject();
            }
            return nullptr;
        }

        bool isBound() const
        {
            iDelegateInstance* ptr = super::getDelegateInstanceProtected();
            return ptr && ptr->isSafeToExecute();
        }

        bool isBoundToObject(const void* inObject) const
        {
            if (!inObject)
            {
                return false;
            }

            iDelegateInstance* ptr = super::getDelegateInstanceProtected();
            return ptr && ptr->hasSameObject(inObject);
        }

        fDelegateHandle getHandle() const
        {
            fDelegateHandle result;
            if (iDelegateInstance* ptr = super::getDelegateInstanceProtected())
            {
                result = ptr->getHandle();
            }
            return result;
        }
    };
}

inline void* operator new(size_t size, M::fDelegateBase& base)
{
    return base.allocate(size);
}

#endif //MADOKA_DELEGATEBASE_H
