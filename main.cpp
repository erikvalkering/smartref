#include <iostream>
#include <vector>

#include "using.h"

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

    Property<Foo> p_foo;
    p_foo.bar();
}
