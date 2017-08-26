#include <smartref.h>
#include "foobar.h"

#include <iostream>
#include <vector>
#include <typeinfo>

using smartref::using_;

template<typename T>
class Property : public using_<T>
{
public:
    operator T &()
    {
        return data;
    }

private:
    T data;
};

/*
class JSONValue : public using_<JSONValue, double>,
                  public using_<JSONValue, std::string>,
                  public using_<JSONValue, std::vector<JSONValue>>,
                  public using_<JSONValue, std::map<JSONValue, JSONValue>>
{
public:
    operator double &()
    {
        return get<double>(data);
    }

    operator double &()
    {
        return get<std::string>(data);
    }

    operator double &()
    {
        return get<std::vector<JSONValue>>(data);
    }

    operator double &()
    {
        return get<std::map<JSONValue, JSONValue>>(data);
    }

private:
    std::variant<
        double,
        std::string,
        std::vector<JSONValue>,
        std::map<JSONValue, JSONValue>
    > data;
};
*/

int main()
{
/*
    JSONValue json = {};

    json["asdf"] = 1.0;
    json[123456] = nullptr;
    json.DOT(qwerty) = "the other (third) operator dot proposal";
*/

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

    Property<foobar::Foo> foo;
    foo.foo();

    Property<foobar::Bar> bar;
    bar.bar();
    bar.bar2();
    bar.bar3();

    Property<foobar::Baz> baz;
    baz.baz();
    baz.baz2();

    Property<foobar::Bat> bat;
    bat.bat();
    bat.bat2();

    {
        foobar::Bar bar;
        bar.bar();
        bar.bar2();
        bar.bar3();

        foobar::Baz baz;
        baz.baz();
        baz.baz2();

        foobar::Bat bat;
        bat.bat();
        bat.bat2();
    }

    {
        Property<foobar::Bla> bla;
        bla.foo();
        bla.bar();
        auto x = decltype(bla)::baz{1234};
        std::cout << typeid(x).name() << " " << x << std::endl;
        auto y = decltype(bla)::bla{};
        y.foo();
        y.bar();
    }

    {
        Property<foobar::Overloads> o;
        o.foo();
        o.foo(0);
        o.bar<int>();
    }

    {
        Property<foobar::GenericClassA> a;
        a.foobar();
        a.foobar(1);
        a.foobar(1.0);
        static_assert(std::is_same<decltype(a)::some_type, GenericClassA::some_type>::value);

        Property<foobar::GenericClassB> b;
        b.foobar();
        b.foobar(1);
        static_assert(std::is_same<decltype(b)::some_type, GenericClassB::some_type>::value);
    }
}
