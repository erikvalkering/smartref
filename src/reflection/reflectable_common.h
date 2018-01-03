#pragma once

#include <utils/utils.h>

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

} // namespace reflection

#define REFLECTION_REFLECTABLE_ADD_MEMBER_TYPE_REFLECTOR(ReflectorClassName, member)    \
    template<typename F>                                                                \
    class ReflectorClassName                                                            \
        : public reflection::reflect_base<reflection::reflected_kind::member_type>      \
    {                                                                                   \
    public:                                                                             \
        using member = typename F::Class::member;                                       \
    };                                                                                  \
                                                                                        \
    template<typename F>                                                                \
    using detect_is_member_type = decltype(                                             \
        std::declval<typename F::Class::member>())                                      \

// TODO:  Get rid of code duplication!!!
#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR_NON_TEMPLATE(ReflectorClassName, member)   \
    template<typename Forwarder>                                                                        \
    class ReflectorClassName                                                                            \
        : public reflection::reflect_base<reflection::reflected_kind::member_function>                  \
    {                                                                                                   \
    private:                                                                                            \
        decltype(auto) indirect()                                                                       \
        {                                                                                               \
            auto f = [](auto &obj)                                                                      \
            {                                                                                           \
                /* TODO: What if *this was an rvalue, then it should be auto &&obj */                   \
                return obj.member();                                                                    \
            };                                                                                          \
                                                                                                        \
            return Forwarder{}(*this, f);                                                               \
        }                                                                                               \
                                                                                                        \
    public:                                                                                             \
        auto member() -> decltype(indirect())                                                           \
        {                                                                                               \
            return indirect();                                                                          \
        }                                                                                               \
    }                                                                                                   \

#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR(ReflectorClassName, member)                \
    template<typename Forwarder>                                                                        \
    class ReflectorClassName                                                                            \
        : public reflection::reflect_base<reflection::reflected_kind::member_function>                  \
    {                                                                                                   \
    private:                                                                                            \
        template<typename... ExplicitArgs>                                                              \
        struct Indirect;                                                                                \
                                                                                                        \
        template<>                                                                                      \
        struct Indirect<>                                                                               \
        {                                                                                               \
            /* TODO: What if *this was an rvalue, then it should be auto &&obj */                       \
            template<typename Obj, typename... Args>                                                    \
            auto operator()(Obj &obj, Args &&... args)                                                  \
                -> decltype(obj.member(std::forward<Args>(args)...))                                    \
            {                                                                                           \
                return obj.member(std::forward<Args>(args)...);                                         \
            };                                                                                          \
        };                                                                                              \
                                                                                                        \
        template<typename... ExplicitArgs>                                                              \
        struct Indirect<typename... ExplicitArgs>                                                       \
        {                                                                                               \
            template<typename Obj, typename... Args>                                                    \
            auto operator()(Obj &obj, Args &&... args)                                                  \
                -> decltype(obj.template member<ExplicitArgs...>(                                       \
                        std::forward<Args>(args)...))                                                   \
            {                                                                                           \
                return obj.template member<ExplicitArgs...>(                                            \
                    std::forward<Args>(args)...);                                                       \
            };                                                                                          \
        };                                                                                              \
                                                                                                        \
    public:                                                                                             \
        template<typename... ExplicitArgs, typename... Args>                                            \
        auto member(Args &&... args)                                                                    \
            -> decltype(Forwarder{}(*this, Indirect<ExplicitArgs...>{}, std::forward<Args>(args)...))   \
        {                                                                                               \
            return Forwarder{}(*this, Indirect<ExplicitArgs...>{}, std::forward<Args>(args)...);        \
        }                                                                                               \
    }                                                                                                   \
