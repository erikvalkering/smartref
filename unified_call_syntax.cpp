#include <iostream>
#include <string>

using namespace std;

template<typename T, typename...>
struct DelayedImpl {using type = T;};

template<typename T, typename... Dependents>
using Delayed = typename DelayedImpl<T, Dependents...>::type;

template<typename Delay>
struct uniformly_callable2
{
    template<typename Obj, typename T>
    friend auto call(Obj &obj, T x) -> decltype(say(obj, x))
    {
        return say(obj, x);
    }
};

template<typename CRTP, typename Delay = void>
struct uniformly_callable : uniformly_callable2<Delay>
{
    template<typename T>
    auto say(T x) -> decltype(call(static_cast<Delayed<CRTP, T> &>(*this), x))
    {
        return call(static_cast<Delayed<CRTP, T> &>(*this), x);
    }
};

template<typename Delay = void>
struct Person : uniformly_callable<Person<Delay>, Delay>
{
};

void say(Person<> &, string x)
{
    cout << x << endl;
}

Person() -> Person<>;

struct Human : uniformly_callable<Human>
{
};

void say(Human &, string x)
{
    cout << x << endl;
}

int main()
{
    auto person = Person{};
    say(person, "Hi!");
    person.say("Hello!");

    auto human = Human{};
    say(human, "Hi!");
    human.say("Hello!");
}

// TODO: Create a working test using the smartref library,
//       and proof that this technique is possible without
//       making Person a class template. This is only necessary
//       for making it work with true reflection.
//       Also note the delayed lookup mechanism that the
//       "smart references through delegation" paper proposes
//       might be interesting for further study, as it might
//       turn out we need a more general mechanism to delay
//       lookups (i.e. outside of ': using T').
