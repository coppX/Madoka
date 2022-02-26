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

    typedef alignBytes<16, 16> delegateType;
    class delegateBase
    {
    public:
        explicit delegateBase()
            : delegateSize(0)
        {}

        ~delegateBase()
        {
            unBind();
        }
        delegateBase(delegateBase&& other)
        {
            delegateAllocator.moveToEmpty(other.delegateAllocator);
            delegateSize = other.delegateSize;
            other.delegateSize = 0;
        }
        delegateBase& operator=(delegateBase&& other)
        {
            unBind();
            delegateAllocator.moveToEmpty(other.delegateAllocator);
            delegateSize = other.delegateSize;
            other.delegateSize = 0;
            return *this;
        }
        void unBind()
        {
            if (delegateInstance* instance = getDelegateInstanceProtected())
            {
                instance->~delegateInstance();
                delegateAllocator.resizeAllocation(0, 0, sizeof(delegateType));
                delegateSize = 0;
            }
        }

        delegateInstance* getDelegateInstanceProtected() const
        {
            return delegateSize ? (delegateInstance*)delegateAllocator.getAllocation() : nullptr;
        }

        void* allocate(int32_t size)
        {
            if (delegateInstance* instance = getDelegateInstanceProtected())
            {
                instance->~delegateInstance();
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
}
inline void* operator new(size_t size, M::delegateBase& base)
{
    return base.allocate(size);
}

#endif //MADOKA_DELEGATEBASE_H
