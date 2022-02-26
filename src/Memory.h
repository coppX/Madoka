//
// Created by FDC on 2022/1/25.
//

#ifndef MADOKA_MEMORY_H
#define MADOKA_MEMORY_H
#include <atomic>
#include <type_traits>

namespace M
{
    template<typename T>
    struct default_delete
    {

    };

    template<typename T, typename = void>
    struct has_pointer_type : std::false_type {};

    template<typename T>
    struct has_pointer_type<T, typename std::void_t<typename T::pointer>::type> : std::true_type {};

    namespace pointer_type_impl
    {
        template<typename T, typename D, bool = has_pointer_type<D>::value>
        struct pointer_type
        {
            typedef typename D::pointer type;
        };

        template<typename T, typename D>
        struct pointer_type<T, D, false>
        {
            typedef T* type;
        };
    }

    template<typename T, typename D>
    struct pointer_type
    {
        typedef typename pointer_type_impl::pointer_type<T, typename std::remove_reference<D>::type>::type type;
    };

    template<typename T, typename D = default_delete<T>>
    class unique_ptr
    {
    public:
        typedef T   element_type;
        typedef D   deleter_type;
        typedef typename pointer_type<T, D>::type pointer;

        // constructor
        explicit unique_ptr(pointer ptr) noexcept;
        unique_ptr(unique_ptr&& other) noexcept;

        template<typename U, typename E>
        unique_ptr(unique_ptr<U, E>&& other) noexcept;

        ~unique_ptr();

        // operator=
        unique_ptr& operator=(unique_ptr&& other) noexcept;

        template<typename U, typename E>
        unique_ptr& operator=(unique_ptr<U, E>&& other) noexcept;

        // release
        pointer release() noexcept;

        // reset
        void reset(pointer ptr = pointer()) noexcept;

        template<typename U>
        void reset(U) noexcept;

        // swap
        void swap(unique_ptr& other) noexcept;

        // get
        pointer get() const noexcept;

        // get_deleter
        deleter_type get_deleter();
        const deleter_type get_deleter() const;

        // operator bool
        explicit operator bool() const noexcept;
        
    private:
        element_type* mPtr;
        deleter_type  mDeleter;
    };

    template<typename T>
    class shared_ptr
    {
    public:
        typedef T element_type;
        // constructor
        shared_ptr() noexcept
            : mPtr{nullptr}
            , mRef{0}
        {
        }

        explicit shared_ptr(T* ptr)
            : mPtr{ptr}
            , mRef{1}
        {

        }

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
#endif //MADOKA_MEMORY_H
