#pragma once

#include "counter.h"
#include "utils.h"

#include <tuple>

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

// A helper type trait to be used in an std::enable_if.
// The template parameter is an expression that is supposed to be
// a valid typename (e.g. 'int' or 'typename vector<int>::value_type'.
// If it is not a valid expression, SFINAE should kick in, and reject
// the candidate template.
template<typename T>
constexpr auto is_typename_v = std::is_same<T, T>::value;

template<typename... Ts>
constexpr auto always_true = true;

namespace detail {

template<typename Class, typename index_pack>
struct Reflection;

template<typename Class, size_t... indices>
struct Reflection<Class, std::index_sequence<indices...>>
{
    auto members() const
    {
        return std::tuple<reflected_member_t<Class, indices>...>{};
    }
};

} // namespace detail

template<typename Class>
using Reflection = detail::Reflection<
    Class,
    std::make_index_sequence<reflected_member_count_v<Class>>
>;

template<typename Class>
constexpr auto reflect = Reflection<Class>{};

template<class Reflection, typename F>
constexpr static auto reify(Reflection, F)
{
    if constexpr (utils::is_detected_v<Reflection::template detect_is_member_type, F>)
    {
        return typename Reflection::template reflect_member_type<F>{};
    }
    else if constexpr (always_true<F>)
    {
        //! Member-functions currently cannot be detected (yet).
        //! However, that is not a problem, because they will be SFINAE'ed away,
        //! in case the reflected entity wasn't a member-function.
        return typename Reflection::template reflect_member_function<F>{};
    }
}

enum class reflected_kind
{
    unknown,
    member_type,
    member_function,
};

struct access
{
    template<typename T>
    constexpr static auto reflected_kind_v =
        decltype(reify(T{}, nullptr))::reflected_kind;
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

} // namespace reflection

#define REFLECTION_REFLECT_COMMON_MEMBER_FUNCTION_REFLECTOR(ReflectorClassName, member, call_member, F)     \
    class ReflectorClassName                                                                                \
        : public reflection::reflect_base<reflection::reflected_kind::member_function>                      \
    {                                                                                                       \
    private:                                                                                                \
        template<typename... ExplicitArgs, typename... Args>                                                \
        decltype(auto) indirect(Args &&... args)                                                            \
        {                                                                                                   \
            auto f = [](auto &obj, auto &&... args)                                                         \
            {                                                                                               \
                /* TODO: What if *this was an rvalue, then it should be auto &&obj */                       \
                if constexpr (sizeof...(ExplicitArgs) == 0)                                                 \
                    return obj.call_member(std::forward<Args>(args)...);                                    \
                else if constexpr (reflection::always_true<Args...>)                                        \
                    return obj.template call_member<ExplicitArgs...>(                                       \
                        std::forward<Args>(args)...);                                                       \
            };                                                                                              \
                                                                                                            \
            return F{}(*this, f, std::forward<Args>(args)...);                                              \
        }                                                                                                   \
                                                                                                            \
    public:                                                                                                 \
        template<typename... ExplicitArgs, typename... Args>                                                \
        auto member(Args &&... args)                                                                        \
            -> decltype(indirect<ExplicitArgs...>(std::forward<Args>(args)...))                             \
        {                                                                                                   \
            return indirect<ExplicitArgs...>(std::forward<Args>(args)...);                                  \
        }                                                                                                   \
    }                                                                                                       \

#define REFLECTION_REFLECT_NONINTRUSIVE(Class, member)                      \
    template<>                                                              \
    struct reflection::reflected_member<Class, CURRENT_COUNTER(Class)>      \
    {                                                                       \
        using type = struct                                                 \
        {                                                                   \
            template<typename F>                                            \
            class reflect_member_type                                       \
                : public reflect_base<reflected_kind::member_type>          \
            {                                                               \
            public:                                                         \
                using member = typename utils::Delayed<Class, F>::member;   \
            };                                                              \
                                                                            \
            template<typename T>                                            \
            using detect_is_member_type = decltype(                         \
                std::declval<typename utils::Delayed<Class, T>::member>()); \
                                                                            \
            template<typename F>                                            \
            REFLECTION_REFLECT_COMMON_MEMBER_FUNCTION_REFLECTOR(            \
                reflect_member_function,                                    \
                member,                                                     \
                member,                                                     \
                F);                                                         \
        };                                                                  \
    };                                                                      \
                                                                            \
    INC_COUNTER(Class)                                                      \

// TODO: REFLECT currently doesn't support member-functions declared using 'auto' --> workaround: REFLECTION_REFLECT_NONINTRUSIVE
// TODO: REFLECT currently doesn't support member-functions templates --> workaround: REFLECTION_REFLECT_NONINTRUSIVE
// TODO: REFLECT currently doesn't support member-functions declared using 'virtual' --> workaround: REFLECTION_REFLECT_NONINTRUSIVE
#define REFLECTION_REFLECT_INTRUSIVE_IMPL(name)                                                         \
    template<typename F>                                                                                \
    REFLECTION_REFLECT_COMMON_MEMBER_FUNCTION_REFLECTOR(                                                \
        __reflect__##name,                                                                              \
        name,                                                                                           \
        __reflect_impl_##name,                                                                          \
        F);                                                                                             \
                                                                                                        \
    template<typename Counter, class Class>                                                             \
    struct __type##name : Counter                                                                       \
    {                                                                                                   \
        template<typename F>                                                                            \
        using reflect = __reflect__##name<F>;                                                           \
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

#define INJECT_CODE_MEMBER_FUNCTION(name, injection_function)                                           \
    __reflect_tag_##name() {}                                                                           \
                                                                                                        \
    injection_function(name);                                                                           \
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
        return utils::delayed<Args...>(*this).__reflect_impl_##name(std::forward<Args>(args)...);       \
    }                                                                                                   \
                                                                                                        \
    /* Here, we define a member-function that will contain the implementation                       */  \
    /* of the reflected member-function.                                                            */  \
    auto __reflect_impl_##name                                                                          \

#define REFLECTION_REFLECT_INTRUSIVE(name)                                  \
    INJECT_CODE_MEMBER_FUNCTION(name, REFLECTION_REFLECT_INTRUSIVE_IMPL)    \

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
