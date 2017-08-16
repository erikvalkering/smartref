#pragma once

#include "counter.h"
#include "utils.h"

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

enum class reflected_kind
{
    unknown,
    member_function,
    member_type,
};

struct access
{
    template<typename T>
    constexpr static auto reflected_kind_v =
        T::template reflect<void>::reflected_kind;

    // TODO: This name is temporary; find a better solution
    template<typename T>
    constexpr static auto reflected_kind2_v =
        T::reflected_kind;
};

template<auto reflected_kind_>
class reflect_base
{
private:
    friend class reflection::access;
    constexpr static auto reflected_kind = reflected_kind_;
};

template<typename T>
constexpr auto reflected_kind_v = access::reflected_kind_v<T>;
template<typename T>
constexpr auto reflected_kind2_v = access::reflected_kind2_v<T>;

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

// TODO: Copy-pasted from smartref.h --> factor out
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

// A helper type trait to be used in an std::enable_if.
// The template parameter is an expression that is supposed to be
// a valid typename (e.g. 'int' or 'typename vector<int>::value_type'.
// If it is not a valid expression, SFINAE should kick in, and reject
// the candidate template.
template<typename T>
constexpr auto is_typename_v = std::is_same<T, T>::value;

} // namespace reflection

// TODO: See if we can rewrite this macro using C++17 if constexpr
#define REFLECTION_REFLECT_NONINTRUSIVE(Class, member)                                          \
    template<>                                                                                  \
    struct reflection::reflected_member<Class, CURRENT_COUNTER(Class)>                          \
    {                                                                                           \
        using type = struct                                                                     \
        {                                                                                       \
            template<typename F, reflected_kind kind>                                           \
            class reflect_member_function                                                       \
                : public reflect_base<kind>                                                     \
            {                                                                                   \
            };                                                                                  \
                                                                                                \
            template<typename F>                                                                \
            class reflect_member_function<F, reflected_kind::unknown>                           \
                : public reflect_base<reflected_kind::member_function>                          \
            {                                                                                   \
            private:                                                                            \
                template<typename... Args>                                                      \
                decltype(auto) indirect(Args &&... args)                                        \
                {                                                                               \
                    auto f = [](auto &obj, auto &&... args)                                     \
                    {                                                                           \
                        /* TODO: What if *this was an rvalue, then it should be auto &&obj */   \
                        return obj.member(std::forward<Args>(args)...);                         \
                    };                                                                          \
                                                                                                \
                    return F{}(*this, f, std::forward<Args>(args)...);                          \
                }                                                                               \
                                                                                                \
            public:                                                                             \
                template<typename... Args>                                                      \
                auto member(Args &&... args) -> decltype(indirect(std::forward<Args>(args)...)) \
                {                                                                               \
                    return indirect(std::forward<Args>(args)...);                               \
                }                                                                               \
            };                                                                                  \
                                                                                                \
            template<typename F, typename = void>                                               \
            class reflect_conditional                                                           \
                : public reflect_base<reflected_kind::unknown> {};                              \
                                                                                                \
            template<typename F>                                                                \
            class reflect_conditional<F, std::enable_if_t<is_typename_v<                        \
                                                          typename Delayed<Class, F>::member>>> \
                : public reflect_base<reflected_kind::member_type>                              \
            {                                                                                   \
            public:                                                                             \
                using member = typename Delayed<Class, F>::member;                              \
            };                                                                                  \
                                                                                                \
            template<typename F>                                                                \
            struct reflect                                                                      \
                : public reflect_conditional<F>                                                 \
                , public reflect_member_function<F, reflected_kind2_v<reflect_conditional<F>>>  \
            {                                                                                   \
            };                                                                                  \
        };                                                                                      \
    };                                                                                          \
                                                                                                \
    INC_COUNTER(Class)                                                                          \

// TODO: REFLECT currently doesn't support member-functions declared using 'auto' --> workaround: REFLECTION_REFLECT_NONINTRUSIVE
// TODO: REFLECT currently doesn't support member-functions templates --> workaround: REFLECTION_REFLECT_NONINTRUSIVE
// TODO: REFLECT currently doesn't support member-functions declared using 'virtual' --> workaround: REFLECTION_REFLECT_NONINTRUSIVE
#define REFLECTION_REFLECT_INTRUSIVE(name)                                                              \
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

//! Define a set of overloads such that we can use REFLECT both inside a class,
//! as well as outside, by dispatching on the number of arguments.
//!
//! REFLECTION_REFLECT(bar) -> REFLECTION_REFLECT_INTRUSIVE(bar)
//! REFLECTION_REFLECT(Foo, bar) -> REFLECTION_REFLECT_NONINTRUSIVE(Foo, bar)
#define REFLECTION_MAKE_OVERLOAD(_1, _2, NAME, ...) NAME
#define REFLECTION_REFLECT(...)             \
    REFLECTION_MAKE_OVERLOAD(               \
        __VA_ARGS__,                        \
        REFLECTION_REFLECT_NONINTRUSIVE,    \
        REFLECTION_REFLECT_INTRUSIVE        \
    )(__VA_ARGS__)                          \

#ifndef REFLECTION_USE_PREFIX
#define REFLECT(...) REFLECTION_REFLECT(__VA_ARGS__)
#endif
