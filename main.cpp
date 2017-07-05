#include <iostream>
#include <vector>

#include "using.h"
#include "foobar.h"

using using_delegate::using_;

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

    Property<foobar::Foo> foo;
    foo.foo();

    Property<foobar::Bar> bar;
    bar.bar();
    bar.bar2();
    bar.bar3();
}
