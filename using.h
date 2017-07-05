#pragma once

#include "reflection.h"

namespace using_delegate {

template<class Derived, typename Delegate>
class using_;

template<template<typename, typename> class Class, class Derived, typename Delegate>
auto DerivedType(Class<Derived, Delegate> &) -> Derived;

template<template<typename, typename> class Class, class Derived, typename Delegate>
auto DelegateType(Class<Derived, Delegate> &) -> Delegate;

template<class Base>
auto &delegate(Base &base)
{
    using Derived = decltype(DerivedType(base));
    static_assert(std::is_base_of_v<Base, Derived>);

    //! Downcast to the derived class
    auto &derived = static_cast<Derived &>(base);

    using Delegate = decltype(DelegateType(base));

    //! Now invoke the conversion operator
    return static_cast<Delegate &>(derived);
}

template<class Class, typename... T>
struct DelayedImpl
{
    using type = Class;
};

template<class Class, typename... T>
using Delayed = typename DelayedImpl<Class, T...>::type;

#define USING_MEMBER(member)                                                                                                                \
    template<typename... T>                                                                                                                 \
    auto member(T &&... args) -> decltype(DelegateType(std::declval<Delayed<decltype(*this), T...>>()).member(std::forward<T>(args)...))    \
    {                                                                                                                                       \
        return delegate(*this).member(std::forward<T>(args)...);                                                                            \
    }                                                                                                                                       \

template<class Derived, typename Delegate>
struct Forwarder
{
    template<typename Self, typename MemberPointer, typename... Args>
    auto operator()(Self &self, MemberPointer member_pointer, Args &&... args)
    {
        static_assert(std::is_base_of_v<Self, Derived>);

        //! Downcast to the derived class
        auto &derived = static_cast<Derived &>(self);

        //! Now invoke the conversion operator
        auto &delegate = static_cast<Delegate &>(derived);

        return delegate.*member_pointer(std::forward<decltype(args)>(args)...);
    }
};

template<class Derived, typename Delegate, size_t index>
using using_member_t = typename reflection::reflected_member_t<Delegate, index>::template reflect<Forwarder<Derived, Delegate>>;

template<class Derived, typename Delegate>
struct MemberFunctions {};

template<class Derived, typename Delegate>
struct STL
{
    USING_MEMBER(push_back)
    USING_MEMBER(begin)
    USING_MEMBER(end)
};

// TODO: using a virtual conversion operation it would be possible to not require CRTP.
//       can it be implemented such that this doesn't add a runtime penalty?
//       what about not using a virtual function, and somehow detect at compile time
//       the derived class.
template<class Derived, typename Delegate>
class using_ : public MemberFunctions<Derived, Delegate>,
               public STL<Derived, Delegate>
{
};

} // namespace using_delegate
