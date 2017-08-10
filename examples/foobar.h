#pragma once

#include <smartref.h>
#include <reflection.h>

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

struct Bla
{
    void foo()
    {
        std::cout << "Bla::foo" << std::endl;
    }

    void REFLECT(bar)()
    {
        std::cout << "Bla::bar" << std::endl;
    }

    using baz = int;
    using bla = Bla;
};

} // namespace foobar

template<typename Derived>
struct smartref::MemberFunctions<foobar::Foo, Derived>
{
    USING_MEMBER(foo)
};

namespace smartref {

// TODO: This currently needs to be declared in the smartref namespace.
//       Figure out a way that it can be declared within an arbitrary namespace.
DECLARE_USING_MEMBER_TYPE(baz);

template<typename Derived>
struct ::smartref::MemberFunctions<foobar::Bla, Derived>
  : USING_MEMBER_TYPE(foobar::Bla, baz)
{
};

} // namespace smartref

REFLECT(foobar::Bar, bar);
REFLECT(foobar::Bar, bar2);
REFLECT(foobar::Bar, bar3);

REFLECT(foobar::Bla, foo); // Member-function
REFLECT(foobar::Bla, bla); // Member-type
