#pragma once

#include <smartref.h>
#include <reflection.h>

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

template<typename Derived>
struct smartref::Members<foobar::Foo, Derived>
{
    USING_MEMBER(foo)
};

namespace smartref {

// TODO: This currently needs to be declared in the smartref namespace.
//       Figure out a way that it can be declared within an arbitrary namespace.
template<typename Derived>
struct ::smartref::Members<foobar::Bla, Derived>
{
    using baz = typename foobar::Bla::baz;
};

} // namespace smartref

REFLECT(foobar::Bar, bar);
REFLECT(foobar::Bar, bar2);
REFLECT(foobar::Bar, bar3);

REFLECT(foobar::Bla, foo); // Member-function
REFLECT(foobar::Bla, bla); // Member-type

REFLECT(foobar::Overloads, foo);
REFLECT(foobar::Overloads, bar);

// TODO: Come up with a mechanism to support 'auto' syntax
REFLECT(reflection::auto_, foobar);
REFLECT(reflection::auto_, some_type);
REFLECT(reflection::auto_, foobarbaz);
REFLECT(reflection::auto_, some_foo_type);

static_assert(std::tuple_size<decltype(members(reflection::reflect<reflection::auto_>))>::value == 4);
