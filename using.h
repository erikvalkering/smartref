#pragma once

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

#define MEMBER(member)                                                                          \
    template<typename... T>                                                                     \
    auto member(T &&... args) -> decltype(DelegateType(std::declval<Delayed<decltype(*this), T...>>()).member(std::forward<T>(args)...)) \
    {                                                                                           \
        return delegate(*this).member(std::forward<T>(args)...);                                \
    }                                                                                           \

template<class Derived, typename Delegate>
struct MemberFunctions {};

template<class Derived, typename Delegate>
struct STL
{
    MEMBER(push_back)
    MEMBER(begin)
    MEMBER(end)
};

template<class Derived, typename Delegate>
class using_ : public MemberFunctions<Derived, Delegate>,
               public STL<Derived, Delegate>
{
};

} // namespace using_delegate
