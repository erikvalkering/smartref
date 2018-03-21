#include <iostream>
#include <string>

using namespace std;

template<typename T, typename...>
struct DelayedImpl {using type = T;};

template<typename T, typename... Dependents>
using Delayed = typename DelayedImpl<T, Dependents...>::type;

template<typename CRTP, typename Delay>
struct extendable2
{
    template<typename Obj, typename T>
    friend auto say_(Obj &obj, T x) -> decltype(say(obj, x))
    {
        return say(obj, x);
    }
};

template<typename CRTP, typename Delay>
struct extendable : extendable2<extendable<CRTP, Delay>, Delay>
{
    template<typename T>
    auto say(T x) -> decltype(say_(static_cast<Delayed<CRTP, T> &>(*this), x))
    {
        return say_(static_cast<Delayed<CRTP, T> &>(*this), x);
    }
};

template<typename Delay = void>
struct Person : extendable<Person<Delay>, Delay>
{
};

void say(Person<> &, string x)
{
    cout << x << endl;
}

int main()
{
    auto person = Person<>{};

    say(person, "Hi!");

    person.say("Hello!");
}
