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

template<typename... T, typename Arg>
decltype(auto) delayed(Arg &&arg)
{
    return std::forward<Arg>(arg);
}

template<typename L1, typename L2>
struct Combiner : L1, L2
{
    constexpr Combiner(L1 l1, L2 l2) : L1(std::move(l1)), L2(std::move(l2)) {}

    using L1::operator();
    using L2::operator();
};

template<typename L1, typename L2>
constexpr auto make_combiner(L1 &&l1, L2 &&l2)
{
    return Combiner<std::decay_t<L1>, std::decay_t<L2>>{std::forward<L1>(l1), std::forward<L2>(l2)};
}

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
#define REFLECT(name)                                                                                       \
    __reflect_tag_##name() {}                                                                               \
                                                                                                            \
    static constexpr auto __reflector_mine = [](auto counter) -> decltype(__reflectXY(counter))             \
    {                                                                                                       \
        return __reflectXY(counter);                                                                        \
    };                                                                                                      \
    static constexpr auto __reflector_fallback = [](counter::Counter<void, 0>)                              \
    {                                                                                                       \
        return counter::Counter<void, 0>{};                                                                 \
    };                                                                                                      \
                                                                                                            \
    static constexpr auto __reflector = reflection::make_combiner(__reflector_mine, __reflector_fallback);  \
                                                                                                            \
    static constexpr const int value_42 = counter::current_value<void>(__reflector); \
    static_assert(value_42 == 0);                                                                           \
                                                                                                            \
    static constexpr counter::Counter<void, 10> __reflectXY(counter::Counter<void, 10>) {return {};}        \
                                                                                                            \
    static constexpr auto __reflector_mine2 = [](auto counter) -> decltype(__reflectXY(counter))            \
    {                                                                                                       \
        return __reflectXY(counter);                                                                        \
    };                                                                                                      \
                                                                                                            \
    static constexpr auto __reflector2 = reflection::make_combiner(__reflector_mine2, __reflector_fallback);\
    static constexpr const int value_43 = counter::current_value2<void>(__reflector2);                      \
    static_assert(value_43 == 10);                                                                           \
                                                                                                        \
    /* Here, we basically replace the reflected member-function,                                    */  \
    /* by defining a new member-function template with the same name,                               */  \
    /* which simply forwards to the implementation of the reflect member-function.                  */  \
    template<typename... Args>                                                                          \
    auto name(Args &&... args) -> decltype(__reflect_tag_##name(std::forward<Args>(args)...))           \
    {                                                                                                   \
        /* Because the function containing the implementation is defined                            */  \
        /* after this member function, it is not available at this point.                           */  \
        /* By making the evaluation of '*this' dependent on the template arguments,                 */  \
        /* we can work around this (this is exactly what 'delayed()' does).                         */  \
        return reflection::delayed<Args...>(*this).__reflect_impl_##name(std::forward<Args>(args)...);  \
    }                                                                                                   \
                                                                                                        \
    /* Here, we define a member-function that will contain the implementation                       */  \
    /* of the reflected member-function.                                                            */  \
    auto __reflect_impl_##name                                                                          \

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
