#pragma once

#include "counter.h"

#define REFLECT(name) \
  __reflection_tag_##name() \
  { \
    throw std::logic_error{"__reflection_tag should never be called"}; \
  } \
  \
  struct __reflection_##name \
  { \
  }; \

namespace reflection {

template<typename T, size_t counter>
struct reflected_member
{
    using type = void;
};

template<typename T, size_t counter>
using reflected_member_t = typename reflected_member<T, counter>::type;

template<typename T, size_t count = 0, typename = reflected_member_t<T, count>>
struct reflected_member_count
{
    static constexpr auto value = reflected_member_count<T, count + 1>::value;
};

template<typename T, size_t count>
struct reflected_member_count<T, count, void>
{
    static constexpr auto value = count;
};

template<typename T>
constexpr auto reflected_member_count_v = reflected_member_count<T>::value;

#define REFLECT_MEMBER(Class, member)                       \
    template<>                                              \
    struct reflected_member<Class, CURRENT_COUNTER(Class)>  \
    {                                                       \
        using type = int;                                   \
    };                                                      \
                                                            \
    INC_COUNTER(Class)                                      \

struct Foo {};
static_assert(std::is_same_v<void, reflected_member_t<Foo, 0>>);
static_assert(reflected_member_count_v<Foo> == 0);

struct Bar {};
REFLECT_MEMBER(Bar, TODO);
static_assert(!std::is_same_v<void, reflected_member_t<Bar, 0>>);
static_assert( std::is_same_v<void, reflected_member_t<Bar, 1>>);
static_assert(reflected_member_count_v<Bar> == 1);

struct Baz {};
REFLECT_MEMBER(Baz, TODO);
REFLECT_MEMBER(Baz, TODO);
static_assert(!std::is_same_v<void, reflected_member_t<Baz, 0>>);
static_assert(!std::is_same_v<void, reflected_member_t<Baz, 1>>);
static_assert( std::is_same_v<void, reflected_member_t<Baz, 2>>);
static_assert(reflected_member_count_v<Baz> == 2);

auto _ = []{
    std::cout << "Foo" << std::endl;
    std::cout << reflected_member_count_v<Foo> << std::endl;

    std::cout << "Bar" << std::endl;
    std::cout << reflected_member_count_v<Bar> << std::endl;

    std::cout << "Baz" << std::endl;
    std::cout << reflected_member_count_v<Baz> << std::endl;

    return 0;
}();

} // namespace reflection
