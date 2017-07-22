#include "reflection.h"

#include <iostream>
#include <type_traits>

struct Foo {};
static_assert(std::is_same<void, reflection::reflected_member_t<Foo, 0>>::value);
static_assert(reflection::reflected_member_count_v<Foo> == 0);

struct Bar
{
    void bar();
};
REFLECT_MEMBER(Bar, bar);
static_assert(!std::is_same<void, reflection::reflected_member_t<Bar, 0>>::value);
static_assert( std::is_same<void, reflection::reflected_member_t<Bar, 1>>::value);
static_assert(reflection::reflected_member_count_v<Bar> == 1);

struct Baz
{
    void baz();
    void baz2();
};
REFLECT_MEMBER(Baz, baz);
REFLECT_MEMBER(Baz, baz2);
static_assert(!std::is_same<void, reflection::reflected_member_t<Baz, 0>>::value);
static_assert(!std::is_same<void, reflection::reflected_member_t<Baz, 1>>::value);
static_assert( std::is_same<void, reflection::reflected_member_t<Baz, 2>>::value);
static_assert(reflection::reflected_member_count_v<Baz> == 2);

struct Bat
{
    void REFLECT(bat)();
    void REFLECT(bat2)();
};

static_assert(!std::is_same<void, reflection::reflected_class_member_t<Bat, 0>>::value);
static_assert(!std::is_same<void, reflection::reflected_class_member_t<Bat, 1>>::value);
static_assert( std::is_same<void, reflection::reflected_class_member_t<Bat, 2>>::value);
static_assert(reflection::reflected_class_member_count_v<Bat> == 2);

auto _ = []{
    using namespace reflection;

    std::cout << "Foo" << std::endl;
    std::cout << reflected_member_count_v<Foo> << std::endl;

    std::cout << "Bar" << std::endl;
    std::cout << reflected_member_count_v<Bar> << std::endl;

    std::cout << "Baz" << std::endl;
    std::cout << reflected_member_count_v<Baz> << std::endl;

    return 0;
}();

int main()
{
}
