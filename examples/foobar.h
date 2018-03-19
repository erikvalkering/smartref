#pragma once

#include <smartref/smartref.h>
#include <reflection/reflection.h>
#include <utils/utils.h>

#include <iostream>
#include <typeinfo>

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
    void baz()
    {
        std::cout << "Baz::baz" << std::endl;
    }

    void baz2()
    {
        std::cout << "Baz::baz2" << std::endl;
    }
};

struct Bat
{
    void bat()
    {
        std::cout << "Bat::bat" << std::endl;
    }

    void bat2()
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

    void bar()
    {
        std::cout << "Bla::bar" << std::endl;
    }

    using baz = int;
    using bla = Bla;
};

struct Overloads
{
    void foo()
    {
        std::cout << "Overloads::foo()" << std::endl;
    }

    void foo(int)
    {
        std::cout << "Overloads::foo(int)" << std::endl;
    }

    template<typename T>
    void bar()
    {
        std::cout << "Overloads::bar<" << typeid(T).name() << ">()" << std::endl;
    }
};

struct GenericClassA
{
    void foobar()
    {
        std::cout << "GenericClassA::foobar()" << std::endl;
    }

    template<typename T>
    void foobar(T x)
    {
        std::cout << "GenericClassA::foobar(" << typeid(x).name() << ")" << std::endl;
    }

    using some_type = int;
};

struct GenericClassB
{
    void foobar()
    {
        std::cout << "GenericClassB::foobar()" << std::endl;
    }

    void foobar(int x)
    {
        std::cout << "GenericClassB::foobar(int)" << std::endl;
    }

    using some_type = float;
};

template<typename T>
struct ClassTemplate
{
    void foobarbaz()
    {
        std::cout << "ClassTemplate<" << typeid(T).name() << ">::foobarbaz()" << std::endl;
    }

    using some_foo_type = double;
};

} // namespace foobar

// TODO: -cmaster Get rid of *all* syntaxes that will never be used

REFLECTABLE(foobar::Foo, foo);

REFLECTABLE(foobar::Bar, bar);
REFLECTABLE(foobar::Bar, bar2);
REFLECTABLE(foobar::Bar, bar3);

REFLECTABLE(foobar::Baz, baz)
REFLECTABLE(foobar::Baz, baz2)

REFLECTABLE(foobar::Bat, bat)
REFLECTABLE(foobar::Bat, bat2)

REFLECTABLE(foobar::Bla, foo); // Member-function
REFLECTABLE(foobar::Bla, bar)
REFLECTABLE(foobar::Bla, baz);
REFLECTABLE(foobar::Bla, bla); // Member-type

REFLECTABLE(foobar::Overloads, foo);
REFLECTABLE(foobar::Overloads, bar);

REFLECTABLE(auto, foobar);
REFLECTABLE(auto, some_type);
REFLECTABLE(auto, foobarbaz);
REFLECTABLE(auto, some_foo_type);

//! TODO: This is no longer valid, since we switched to generic reflection for the STL.
//        Create a unit-test that tests this in isolation.
// static_assert(utils::pack_size(members(REFLECTION_REFLECT_AUTO(auto))) == 4);
