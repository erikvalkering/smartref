#pragma once

#include "using.h"
#include "reflection.h"

struct Foo
{
    void foo()
    {
        std::cout << "Foo::foo" << std::endl;
    }
};

template<typename Derived>
struct MemberFunctions<Derived, Foo>
{
    MEMBER(foo)
};

struct Bar
{
    void bar()
    {
        std::cout << "Bar::bar" << std::endl;
    }
};

REFLECT_MEMBER(Bar, bar);

struct Baz
{
    void REFLECT(baz)()
    {
        std::cout << "Baz::baz" << std::endl;
    }
};
