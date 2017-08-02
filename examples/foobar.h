#pragma once

#include "using.h"
#include "reflection.h"

#include <iostream>

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

    void REFLECT(baz2)()
    {
        std::cout << "Baz::baz2" << std::endl;
    }
};

struct Bat
{
    void REFLECT(bat)()
    {
        std::cout << "Bat::bat" << std::endl;
    }

    void REFLECT(bat2)()
    {
        std::cout << "Bat::bat2" << std::endl;
    }
};

} // namespace foobar

template<typename Derived>
struct using_delegate::MemberFunctions<foobar::Foo, Derived>
{
    USING_MEMBER(foo)
};

REFLECT(foobar::Bar, bar);
REFLECT(foobar::Bar, bar2);
REFLECT(foobar::Bar, bar3);
