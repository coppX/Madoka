//
// Created by 最上川 on 2022/2/22.
//

#ifndef MTHREAD_TYPE_TRAITS_H
#define MTHREAD_TYPE_TRAITS_H

namespace M
{
    template<typename T> struct remove_reference        { typedef T type; };
    template<typename T> struct remove_reference<T&>    { typedef T type; };
    template<typename T> struct remove_reference<T&&>   { typedef T type; };

    template<typename _Signature>
    struct result_of;

    template<typename T>
    struct decay
    {
    private:
        typedef typename remove_reference<T>::type Up;
    };
}
#endif //MTHREAD_TYPE_TRAITS_H
