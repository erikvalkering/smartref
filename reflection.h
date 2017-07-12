#pragma once

#include "counter.h"

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

// TODO: Copy-pasted from using.h --> factor out
template<class Class, typename... T>
struct DelayedImpl
{
    using type = Class;
};

template<class Class, typename... T>
using Delayed = typename DelayedImpl<Class, T...>::type;

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

// TODO: REFLECT currently doesn't support member-functions declared using 'auto' --> workaround: REFLECT_MEMBER
// TODO: REFLECT currently doesn't support member-functions templates --> workaround: REFLECT_MEMBER
// TODO: REFLECT currently doesn't support member-functions declared using 'virtual' --> workaround: REFLECT_MEMBER
#define REFLECT(name)                                                                                   \
  __reflect_tag_##name() {}                                                                             \
                                                                                                        \
  struct __reflect_thunker##name                                                                        \
  {                                                                                                     \
      template<typename Self, typename... Args>                                                         \
      static auto thunk(Self &self, Args &&... args)                                                    \
      {                                                                                                 \
          return self.__reflect_thunk_##name(std::forward<Args>(args)...);                              \
      }                                                                                                 \
  };                                                                                                    \
                                                                                                        \
  template<typename... Args>                                                                            \
  auto name(Args &&... args) -> decltype(__reflect_tag_##name(std::forward<Args>(args)...))             \
  {                                                                                                     \
    reflection::Delayed<__reflect_thunker##name, Args...>::thunk(*this, std::forward<Args>(args)...);   \
  }                                                                                                     \
                                                                                                        \
  auto __reflect_thunk_##name                                                                           \

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
