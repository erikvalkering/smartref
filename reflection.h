#pragma once

#include "counter.h"

namespace reflection {

// TODO: Create a constexpr_vector abstraction

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

template<typename T, size_t counter, typename = void>
struct reflected_class_member
{
    using type = void;
};

template<typename T, size_t counter>
struct reflected_class_member<T, counter, std::enable_if_t<decltype(std::declval<T>().__reflect(counter::Counter<counter>{}))::value == counter>>
{
    using type = decltype(std::declval<T>().__reflect(counter::Counter<counter>{}));
};

template<typename T, size_t counter>
using reflected_class_member_t = typename reflected_class_member<T, counter>::type;

// TODO: Get rid of code duplication between reflected_member_count and reflected_class_member_count
template<typename T, size_t count = 0, typename = reflected_class_member_t<T, count>>
struct reflected_class_member_count
{
    static constexpr auto value = reflected_class_member_count<T, count + 1>::value;
};

template<typename T, size_t count>
struct reflected_class_member_count<T, count, void>
{
    static constexpr auto value = count;
};

template<typename T>
constexpr auto reflected_class_member_count_v = reflected_class_member_count<T>::value;

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
    __reflect_tag_##name() {}                                                                           \
                                                                                                        \
    template<typename F, class Class>                                                                   \
    class __reflect__##name                                                                             \
    {                                                                                                   \
    private:                                                                                            \
        template<typename... Args>                                                                      \
        decltype(auto) indirect(Args &&... args)                                                        \
        {                                                                                               \
            return F{}(*this, &Class::__reflect_impl_##name, std::forward<Args>(args)...);              \
        }                                                                                               \
                                                                                                        \
    public:                                                                                             \
        template<typename... Args>                                                                      \
        auto name(Args &&... args) -> decltype(indirect(std::forward<Args>(args)...))                   \
        {                                                                                               \
            return indirect(std::forward<Args>(args)...);                                               \
        }                                                                                               \
    };                                                                                                  \
                                                                                                        \
    template<typename Counter, class Class>                                                             \
    struct __type##name : Counter                                                                       \
    {                                                                                                   \
        template<typename F>                                                                            \
        using reflect = __reflect__##name<F, Class>;                                                    \
    };                                                                                                  \
                                                                                                        \
    auto __reflect(counter::Counter<CURRENT_CLASS_COUNTER()> counter)                                   \
    {                                                                                                   \
        using Class = std::decay_t<decltype(*this)>;                                                    \
                                                                                                        \
        using type = __type##name<decltype(counter), Class>;                                            \
                                                                                                        \
        return type{};                                                                                  \
    }                                                                                                   \
                                                                                                        \
    INC_CLASS_COUNTER();                                                                                \
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

struct Bat
{
    void REFLECT(bat)();
    void REFLECT(bat2)();
};

static_assert(!std::is_same_v<void, reflection::reflected_class_member_t<Bat, 0>>);
static_assert(!std::is_same_v<void, reflection::reflected_class_member_t<Bat, 1>>);
static_assert( std::is_same_v<void, reflection::reflected_class_member_t<Bat, 2>>);
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
