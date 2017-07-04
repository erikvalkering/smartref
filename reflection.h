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

// template<typename T>
// constexpr auto reflected_member_count_v =;

struct Foo {};
static_assert(std::is_same_v<void, reflected_member_t<Foo, 0>>);

struct Bar {};
// template<> struct reflection::reflected_member<Bar, 0> {using type = int;};

struct Baz {};
// template<> struct reflected_member<Baz, CURRENT_COUNTER()> {using type = int;}; INC_COUNTER();
// template<> struct reflected_member<Baz, CURRENT_COUNTER()> {using type = int;}; INC_COUNTER();

// static_assert(reflected_member_count_v<Foo> == 0);
// static_assert(reflected_member_count_v<Bar> == 1);
// static_assert(reflected_member_count_v<Baz> == 2);

auto _ = []{
    // std::cout << "Foo" << std::endl;
    // std::cout << reflected_member_count_v<Foo> << std::endl;

    // std::cout << "Bar" << std::endl;
    // std::cout << reflected_member_count_v<Bar> << std::endl;

    // std::cout << "Baz" << std::endl;
    // std::cout << reflected_member_count_v<Baz> << std::endl;

    // std::cout << "Bat" << std::endl;
    // std::cout << reflected_member_count_v<Bat> << std::endl;

    return 0;
}();

} // namespace reflection
