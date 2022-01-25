//
// Created by FDC on 2022/1/25.
//

#ifndef MTHREAD_MSHAREDPTR_H
#define MTHREAD_MSHAREDPTR_H
#include <atomic>

namespace M
{
    template<typename T>
    class shared_ptr
    {
    public:
        typedef T element_type;
        // constructor
        explicit shared_ptr(T* ptr);

        template<typename T1, typename Deleter>
        shared_ptr(T1* ptr, Deleter del);

        template<typename T1, typename Deleter, typename Allocator>
        shared_ptr(T1* ptr, Deleter del, Allocator alloc);

        shared_ptr(shared_ptr&& r);

        template<typename T1>
        shared_ptr(shared_ptr<T1>&&);

        ~shared_ptr();

        // operator=
        shared_ptr& operator=(const shared_ptr& r);

        template<typename T1>
        shared_ptr& operator=(const shared_ptr<T1>& r);

        template<typename T1>
        shared_ptr& operator=(shared_ptr<T1>&& r);

        // reset
        void reset();

        template<typename T1>
        void reset(T1* p);

        template<typename T1, typename Deleter>
        void reset(T1* ptr, Deleter del);

        template<typename T1, typename Deleter, typename Allocator>
        void reset(T1* ptr, Deleter del, Allocator alloc);

        // swap
        void swap(shared_ptr& r);

        // get
        T* get();

        // operator* operator->
        T& operator*() const noexcept;
        T* operator->() const noexcept;

        // operator[]
        element_type& operator[](std::ptrdiff_t idx) const;

        // use_count
        long use_count() const noexcept;


    private:
        std::atomic<long>    mRef;
        element_type*       mPtr;
    };
}
#endif //MTHREAD_MSHAREDPTR_H
