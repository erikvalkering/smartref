#pragma once

#include "utils.h"

#include <cstddef>

namespace reflection {

template<
    template<typename, size_t> class reflected_member_slot_t,
    typename T,
    size_t count = 0,
    typename = reflected_member_slot_t<T, count>
>
struct reflected_member_count
{
    static constexpr auto value = reflected_member_count<reflected_member_slot_t, T, count + 1>::value;
};

template<
    template<typename, size_t> class reflected_member_slot_t,
    typename T,
    size_t count
>
struct reflected_member_count<reflected_member_slot_t, T, count, void>
{
    static constexpr auto value = count;
};

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

//! A tag-type that can be used to create generic reflections.
struct auto_;

} // namespace reflection

#define REFLECTION_REFLECT_ADD_MEMBER_TYPE_REFLECTOR(ReflectorClassName, member)    \
    template<typename F>                                                            \
    class ReflectorClassName                                                        \
        : public reflection::reflect_base<reflection::reflected_kind::member_type>  \
    {                                                                               \
    public:                                                                         \
        using member = typename F::Class::member;                                   \
    };                                                                              \
                                                                                    \
    template<typename F>                                                            \
    using detect_is_member_type = decltype(                                         \
        std::declval<typename F::Class::member>())                                  \

#define REFLECTION_REFLECT_ADD_MEMBER_FUNCTION_REFLECTOR(ReflectorClassName, member)    \
    template<typename F>                                                                \
    class ReflectorClassName                                                            \
        : public reflection::reflect_base<reflection::reflected_kind::member_function>  \
    {                                                                                   \
    private:                                                                            \
        template<typename... ExplicitArgs, typename... Args>                            \
        decltype(auto) indirect(Args &&... args)                                        \
        {                                                                               \
            auto f = [](auto &obj, auto &&... args)                                     \
            {                                                                           \
                /* TODO: What if *this was an rvalue, then it should be auto &&obj */   \
                if constexpr (sizeof...(ExplicitArgs) == 0)                             \
                    return obj.member(std::forward<Args>(args)...);                     \
                else if constexpr (reflection::always_true<Args...>)                    \
                    return obj.template member<ExplicitArgs...>(                        \
                        std::forward<Args>(args)...);                                   \
            };                                                                          \
                                                                                        \
            return F{}(*this, f, std::forward<Args>(args)...);                          \
        }                                                                               \
                                                                                        \
    public:                                                                             \
        template<typename... ExplicitArgs, typename... Args>                            \
        auto member(Args &&... args)                                                    \
            -> decltype(indirect<ExplicitArgs...>(std::forward<Args>(args)...))         \
        {                                                                               \
            return indirect<ExplicitArgs...>(std::forward<Args>(args)...);              \
        }                                                                               \
    }                                                                                   \
