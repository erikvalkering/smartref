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

    void bar2()
    {
        std::cout << "Bar::bar" << std::endl;
    }

    void bar3()
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
struct using_delegate::MemberFunctions<Derived, foobar::Foo>
{
    USING_MEMBER(foo)
};

REFLECT_MEMBER(foobar::Bar, bar);
REFLECT_MEMBER(foobar::Bar, bar2);
REFLECT_MEMBER(foobar::Bar, bar3);
