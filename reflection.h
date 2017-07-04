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

template<typename default_type, typename T>
struct reflected_member_default
{
    using type = nullptr_t;
};

template<typename T>
using reflected_member_default_t = typename reflected_member_default<void, T>::type;

template<typename T, size_t counter>
struct reflected_member
{
    using type = reflected_member_default_t<T>;
};

// struct A {};
// struct B {};

// template<>
// struct __is_reflected<A> {};

// static_cast(decltype(is_reflected<A>())::value == true);
// static_cast(decltype(is_reflected<A>())::value == false);

template<typename T, size_t counter>
using reflected_member_t = typename reflected_member<T, counter>::type;

template<typename T, size_t index = 0, typename = reflected_member_t<T, 0>>
struct reflected_member_count_lowerbound
{
    static constexpr auto value = index;
};

template<typename T, size_t index>
struct reflected_member_count_lowerbound<T, index, void>
{
    static constexpr auto value = reflected_member_count_lowerbound<T, index, reflected_member_t<T, index + 1>>::value;
};

template<typename T, size_t index = 0, typename = reflected_member_t<T, 0>>
struct reflected_member_count_upperbound
{
    static constexpr auto value = reflected_member_count_upperbound<T, index + 1, reflected_member_t<T, index + 1>>::value;
};

template<typename T, size_t index>
struct reflected_member_count_upperbound<T, index, void>
{
    static constexpr auto value = index;
};

template<typename T>
constexpr auto reflected_member_count_v = reflected_member_count_upperbound<T>::value - reflected_member_count_lowerbound<T>::value;

template<typename T, size_t index>
constexpr auto is_reflected_member = std::is_void_v<reflected_member_t<T, index>>;

template<typename T>
constexpr auto reflected_member_count()
{
    const auto max = 5;

    auto count = size_t{};
    for (size_t index = 0; index != max; ++index)
        if (is_reflected_member<T, index>)
            ++count;

    return count;
}

struct Foo {};
struct Bar {};
struct Baz {};
struct Bat {};

template<typename T>
struct is_reflected
{
    static constexpr auto value = false;
};

template<typename T>
constexpr auto is_reflected_v = is_reflected<T>::value;

template<> struct reflected_member<Bar, 0> {using type = int;}; template<typename default_type> struct reflected_member_default<default_type, Bar> {using type = default_type;};
template<> struct reflected_member<Baz, 0> {using type = int;}; template<typename default_type> struct reflected_member_default<default_type, Baz> {using type = default_type;};
template<> struct reflected_member<Baz, 1> {using type = int;}; template<typename default_type> struct reflected_member_default<default_type, Baz> {using type = default_type;};
template<> struct reflected_member<Bat, 1> {using type = int;}; template<typename default_type> struct reflected_member_default<default_type, Bat> {using type = default_type;};

// static_assert(is_reflected_v<Foo> == false);
// static_assert(is_reflected_v<Bar> == true);
// static_assert(is_reflected_v<Baz> == true);
// static_assert(is_reflected_v<Bat> == true);
// static_assert(reflected_member_count_v<Foo> == 0);
static_assert(reflected_member_count_v<Bar> == 1);
static_assert(reflected_member_count_v<Baz> == 2);
// static_assert(reflected_member_count_v<Bat> == 1);
// static_assert(reflected_member_count<Bar>() == 1);
// static_assert(reflected_member_count<Baz>() == 2);

auto _ = []{
    // std::cout << "Foo" << std::endl;
    // std::cout << reflected_member_count_lowerbound<Foo>::value << std::endl;
    // std::cout << reflected_member_count_upperbound<Foo>::value << std::endl;
    // std::cout << reflected_member_count_v<Foo> << std::endl;

    std::cout << "Bar" << std::endl;
    std::cout << reflected_member_count_lowerbound<Bar>::value << std::endl;
    std::cout << reflected_member_count_upperbound<Bar>::value << std::endl;
    std::cout << reflected_member_count_v<Bar> << std::endl;

    std::cout << "Baz" << std::endl;
    std::cout << reflected_member_count_lowerbound<Baz>::value << std::endl;
    std::cout << reflected_member_count_upperbound<Baz>::value << std::endl;
    std::cout << reflected_member_count_v<Baz> << std::endl;

    std::cout << "Bat" << std::endl;
    std::cout << reflected_member_count_lowerbound<Bat>::value << std::endl;
    std::cout << reflected_member_count_upperbound<Bat>::value << std::endl;
    std::cout << reflected_member_count_v<Bat> << std::endl;
    return 0;
}();

} // namespace reflection
