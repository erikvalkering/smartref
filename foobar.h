#pragma once

#include "using.h"
#include "reflection.h"

namespace foobar {

struct Foo
{
    void foo()
    {
        std::cout << "Foo::foo" << std::endl;
    }
};

struct Bar
{
    void bar()
    {
        std::cout << "Bar::bar" << std::endl;
    }
};

struct Baz
{
    void REFLECT(baz)()
    {
        std::cout << "Baz::baz" << std::endl;
    }
};

} // namespace foobar

template<typename Derived>
struct using_delegate::MemberFunctions<Derived, Foo>
{
    USING_MEMBER(foo)
};

REFLECT_MEMBER(Bar, bar);
