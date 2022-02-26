//
// Created by 最上川 on 2022/2/26.
//

#ifndef MADOKA_ALLOCATOR_H
#define MADOKA_ALLOCATOR_H
#include "Types.h"
#include <memory>
namespace M
{
    template<int32_t size, int32_t align>
    struct alignBytes
    {
        struct MS_ALIGN(align) Padding
        {
            int8_t pad[size];
        } GCC_ALIGN(align);
        Padding padding;
    };

    struct memHandle
    {
    };

    template<int IndexSize>
    struct bitsToSizeType
    {
        static_assert((IndexSize & (IndexSize - 1)) == 0, "Unsupported allocator index size.");
    };
    template<> struct bitsToSizeType<8>  { using type = int8_t;  };
    template<> struct bitsToSizeType<16> { using type = int16_t; };
    template<> struct bitsToSizeType<32> { using type = int32_t; };
    template<> struct bitsToSizeType<64> { using type = int64_t; };

    /** The indirect allocation policy always allocates the elements indirectly. */
    template<int IndexType>
    class sizedHeapAllocator
    {
    public:
        using sizeType = typename bitsToSizeType<IndexType>::type;

        class forAnyElementType
        {
            template<int>
            friend
            class heapAllocator;

        public:
            forAnyElementType()
                    : data(nullptr) {}

            ~forAnyElementType()
            {
                if (data) std::free(data);
            }

            forAnyElementType(const forAnyElementType &) = delete;

            forAnyElementType &operator=(const forAnyElementType &) = delete;

            template<typename OtherAllocator>
            void moveToEmpty(typename OtherAllocator::forAnyElementType &other)
            {
                assert((void*) this != (void*) &other);
                if (data)
                {
                    std::free(data);
                }
                data = other.data;
                other.data = nullptr;
            }

            void moveToEmpty(forAnyElementType &other)
            {
                this->template moveToEmpty<sizedHeapAllocator>(other);
            }

            memHandle* getAllocation() const
            {
                return data;
            }

            bool hasAllocation() const { return !!data; }

            sizeType getInitialCapacity() const { return 0; }

            void resizeAllocation(sizeType previousNumElement, sizeType numElements, size_t numBytesPerElement)
            {
                if (data || numElements)
                {
                    data = (memHandle*) std::realloc(data, numElements * numBytesPerElement);
                }
            }

            size_t getAllocatedSize(sizeType numAllocatedElements, size_t numBytesPerElement) const
            {
                return numAllocatedElements * numBytesPerElement;
            }

        private:
            memHandle* data;
        };

        template<typename elementType>
        class forElementType : public forAnyElementType
        {
        public:
            forElementType() {}

            elementType* getAllocation() const
            {
                return (elementType*) forAnyElementType::getAllocation();
            }
        };
    };

    using heapAllocator = sizedHeapAllocator<32>;
}
#endif //MADOKA_ALLOCATOR_H
