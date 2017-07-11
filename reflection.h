#pragma once

#include "counter.h"

#define REFLECT(name) name

#define REFLECT2(name) \
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

} // namespace reflection

#define REFLECT_MEMBER(Class, member)                                                           \
    template<>                                                                                  \
    struct reflection::reflected_member<Class, CURRENT_COUNTER(Class)>                          \
    {                                                                                           \
        using type = struct                                                                     \
        {                                                                                       \
            template<typename F>                                                                \
            class reflect                                                                       \
            {                                                                                   \
            private:                                                                            \
                template<typename... Args>                                                      \
                decltype(auto) indirect(Args &&... args)                                        \
                {                                                                               \
                    return F{}(*this, &Class::member, std::forward<Args>(args)...);             \
                }                                                                               \
                                                                                                \
            public:                                                                             \
                template<typename... Args>                                                      \
                auto member(Args &&... args) -> decltype(indirect(std::forward<Args>(args)...)) \
                {                                                                               \
                    return indirect(std::forward<Args>(args)...);                               \
                }                                                                               \
            };                                                                                  \
        };                                                                                      \
    };                                                                                          \
                                                                                                \
    INC_COUNTER(Class)                                                                          \

struct Foo {};
static_assert(std::is_same_v<void, reflection::reflected_member_t<Foo, 0>>);
static_assert(reflection::reflected_member_count_v<Foo> == 0);

struct Bar
{
    void bar();
};
REFLECT_MEMBER(Bar, bar);
static_assert(!std::is_same_v<void, reflection::reflected_member_t<Bar, 0>>);
static_assert( std::is_same_v<void, reflection::reflected_member_t<Bar, 1>>);
static_assert(reflection::reflected_member_count_v<Bar> == 1);

struct Baz
{
    void baz();
    void baz2();
};
REFLECT_MEMBER(Baz, baz);
REFLECT_MEMBER(Baz, baz2);
static_assert(!std::is_same_v<void, reflection::reflected_member_t<Baz, 0>>);
static_assert(!std::is_same_v<void, reflection::reflected_member_t<Baz, 1>>);
static_assert( std::is_same_v<void, reflection::reflected_member_t<Baz, 2>>);
static_assert(reflection::reflected_member_count_v<Baz> == 2);

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
