#include <iostream>
#include <vector>

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
class using_
{
public:
    MEMBER(push_back)
    MEMBER(begin)
    MEMBER(end)
};

template<typename T>
class Property : public using_<Property<T>, T>
{
public:
    operator T &()
    {
        return data;
    }

private:
    T data;
};

struct Foo
{
    void bar()
    {
        std::cout << "Foo::bar" << std::endl;
    }
};

int main()
{
    std::cout << "Hello, Wandbox!" << std::endl;

    Property<std::vector<int>> v;

    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    //v.push_back(1, 2, 3);

    for (auto x : v)
    {
        std::cout << x << std::endl;
    }

    Property<int> x{};
    Property<int> y{};

    auto z = x + y;

    std::cout << z << std::endl;
    std::cout << typeid(z).name() << std::endl;

    Property<Foo> p_foo;
    p_foo.bar(); // TODO: Doesn't work yet
}
