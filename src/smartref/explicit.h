#pragma once

#include <utils/utils.h>

#include <type_traits>

namespace smartref {

template<template<typename, typename> class Class, typename Delegate, class Derived>
auto DerivedType(Class<Delegate, Derived> &) -> Derived;

template<template<typename, typename> class Class, typename Delegate, class Derived>
auto DelegateType(Class<Delegate, Derived> &) -> Delegate;

template<class Base>
auto &delegate2(Base &base)
{
    using Derived = decltype(DerivedType(base));
    static_assert(std::is_base_of<Base, Derived>::value);

    //! Downcast to the derived class
    auto &derived = static_cast<Derived &>(base);

    using Delegate = decltype(DelegateType(base));

    //! Now invoke the conversion operator
    return static_cast<Delegate &>(derived);
}

#define USING_MEMBER(member)                                                                                                                    \
    template<typename... T>                                                                                                                     \
    auto member(T &&... args) -> decltype(DelegateType(std::declval<utils::Delayed<decltype(*this), T...>>()).member(std::forward<T>(args)...)) \
    {                                                                                                                                           \
        return delegate2(*this).member(std::forward<T>(args)...);                                                                                \
    }                                                                                                                                           \

template<typename MemberTypeTag, typename Delegate, typename = void>
struct member_type_introducer
{
    template<typename T>
    struct Forwarder
    {
    };
};

#define DECLARE_USING_MEMBER_TYPE(name)                                                     \
    struct tag_##name;                                                                      \
                                                                                            \
    template<typename Delegate>                                                             \
    struct member_type_introducer<tag_##name, Delegate,                                     \
                                  utils::void_t<typename Delegate::name>>                   \
    {                                                                                       \
        template<typename T>                                                                \
        struct Forwarder                                                                    \
        {                                                                                   \
            using name = typename T::name;                                                  \
        };                                                                                  \
    }                                                                                       \

// TODO: We should be able to deduce the Delegate type
#define USING_MEMBER_TYPE(Delegate, name)                                                   \
    smartref::member_type_introducer<tag_##name, Delegate>::template Forwarder<Delegate>    \

template<typename Delegate, class Derived>
struct Members {};

} // namespace smartref
