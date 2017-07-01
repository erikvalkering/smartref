#include <iostream>
#include <vector>

#define MEMBER(member)                                                                               \
    template<typename... T>                                                                          \
    auto member(T &&... args) -> decltype(std::declval<Delegate>().member(std::forward<T>(args)...)) \
    {                                                                                                \
        return delegate().member(std::forward<T>(args)...);                                          \
    }                                                                                                \

template<class Derived, typename Delegate>
class using_ : public MemberFunctions<Delegate>
{
private:
    decltype(auto) delegate()
    {
        //! Downcast to the derived class
        auto &derived = static_cast<Derived &>(*this);

        //! Now invoke the conversion operator
        return static_cast<Delegate &>(derived);
    }

public:
    MEMBER(push_back)
    MEMBER(begin)
    MEMBER(end)
};

template<typename T>
class MyVector : public using_<MyVector<T>, std::vector<T>>
{
public:
    operator std::vector<T> &()
    {
        return data;
    }

    std::vector<T> data;
};

template<typename NumericalType>
class Number
{
public:
    operator NumericalType &()
    {
        return data;
    }

    NumericalType data;
};

int main()
{
    std::cout << "Hello, Wandbox!" << std::endl;

    MyVector<int> v;

    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    //v.push_back(1, 2, 3);

    for (auto x : v)
    {
        std::cout << x << std::endl;
    }

    Number<int> x{};
    Number<int> y{};

    auto z = x + y;

    std::cout << z << std::endl;
    std::cout << typeid(z).name() << std::endl;
}
