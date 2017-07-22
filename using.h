#pragma once

#include "reflection.h"

#include <type_traits>

namespace using_delegate {

template<typename Delegate, class Derived>
class using_;

template<template<typename, typename> class Class, typename Delegate, class Derived>
auto DerivedType(Class<Delegate, Derived> &) -> Derived;

template<template<typename, typename> class Class, typename Delegate, class Derived>
auto DelegateType(Class<Delegate, Derived> &) -> Delegate;

template<class Base>
auto &delegate(Base &base)
{
    using Derived = decltype(DerivedType(base));
    static_assert(std::is_base_of<Base, Derived>::value);

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

template<typename Delegate, class Derived>
struct Forwarder
{
    template<typename Self, typename MemberPointer, typename... Args>
    auto operator()(Self &self, MemberPointer member_pointer, Args &&... args)
    {
        static_assert(std::is_base_of<Self, Derived>::value);

        //! Downcast to the derived class
        auto &derived = static_cast<Derived &>(self);

        //! Now invoke the conversion operator
        auto &delegate = static_cast<Delegate &>(derived);

        return (delegate.*member_pointer)(std::forward<decltype(args)>(args)...);
    }
};

template<typename Delegate, class Derived>
struct MemberFunctions {};

template<typename Delegate, class Derived, size_t index>
using using_member_t = typename reflection::reflected_member_t<Delegate, index>::template reflect<Forwarder<Delegate, Derived>>;

template<typename Delegate, class Derived, typename index_pack>
struct ReflectedMemberFunctionsImpl;

template<typename Delegate, class Derived, size_t... indices>
struct ReflectedMemberFunctionsImpl<Delegate, Derived, std::index_sequence<indices...>>
    : using_member_t<Delegate, Derived, indices>...
{
};

template<typename Delegate, class Derived>
using ReflectedMemberFunctions = ReflectedMemberFunctionsImpl<
    Delegate,
    Derived,
    std::make_index_sequence<reflection::reflected_member_count_v<Delegate>>>;

template<typename Delegate, class Derived, size_t index>
using using_class_member_t = typename reflection::reflected_class_member_t<Delegate, index>::template reflect<Forwarder<Delegate, Derived>>;

template<typename Delegate, class Derived, typename index_pack>
struct ReflectedClassMemberFunctionsImpl;

template<typename Delegate, class Derived, size_t... indices>
struct ReflectedClassMemberFunctionsImpl<Delegate, Derived, std::index_sequence<indices...>>
    : using_class_member_t<Delegate, Derived, indices>...
{
};

template<typename Delegate, class Derived>
using ReflectedClassMemberFunctions = ReflectedClassMemberFunctionsImpl<
    Delegate,
    Derived,
    std::make_index_sequence<reflection::reflected_class_member_count_v<Delegate>>>;

template<typename Delegate, class Derived>
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
template<typename Delegate, class Derived>
class using_ : public MemberFunctions<Delegate, Derived>,
               public STL<Delegate, Derived>,
               public ReflectedMemberFunctions<Delegate, Derived>,
               public ReflectedClassMemberFunctions<Delegate, Derived>
{
};

} // namespace using_delegate
