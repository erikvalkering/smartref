#pragma once

#include "using.h"

struct Foo
{
    void bar()
    {
        std::cout << "Foo::bar" << std::endl;
    }
};

template<typename Derived>
struct MemberFunctions<Derived, Foo>
{
    MEMBER(bar)
};
