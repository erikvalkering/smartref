#include <type_traits>
#include <iostream>

struct foo
{
    void bar() {}
};

struct bat
{
    template<typename>
    void bar() {}
};

struct baz {};

struct BAR
{
    void bar() {}
};

template<typename T>
struct detector2 : T, BAR
{
};

template<typename Obj, auto = &detector2<Obj>::bar>
constexpr auto test2(int) {return false;}
template<typename Obj>
constexpr auto test2(...) {return true;}

/*
template<typename Obj, typename T>
auto call(Obj &&o, T y) -> decltype(o.bar(y), std::true_type{});
auto call(...) -> std::false_type;

constexpr auto detector = [](auto x) { return [&](auto y) { return call(x, y); }; };

template<typename T, typename = decltype(detector(std::declval<T>()))>
auto detected(int) {return true;}
template<typename T>
auto detected(...) {return false;}

struct X
{
    using name = void;
};
*/

//template<typename T>
//struct Y : X
//{

/*
struct Detector
{
    template<typename T, typename U>
    static void name(int) -> decltype(std::declval<T>().bar(std::declval<U>()));

    template<typename T, typename U>
    static void name(...) {};
};

template<typename T>
constexpr has_bar = &Detector<name<T, U
*/

int main()
{
    using std::cout;
    using std::endl;

    cout << "foo has bar: " << test2<foo>(0) << endl;
    cout << "baz has bar: " << test2<baz>(0) << endl;
    cout << "bat has bar: " << test2<bat>(0) << endl;

//    return detected<baz>(0);
}
