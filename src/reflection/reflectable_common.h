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

// TODO: -cmaster Do we actually need / use this enum class?
enum class reflected_kind
{
    unknown,
    member_type,
    member_function,
};

struct access
{
    // TODO: Isn't this only necessary for testing, i.e. accessing the internals to determine the type?
    template<typename T>
    constexpr static auto reflected_kind_v =
        decltype(reify(T{}, nullptr))::reflected_kind;
};

template<class Derived, auto reflected_kind_>
class reflector_base
{
private:
    friend class reflection::access;
    constexpr static auto reflected_kind = reflected_kind_;

    template<typename Self>
    friend decltype(auto) derived(Self &self)
    {
        return static_cast<Derived &>(self);
    }
};

template<typename T>
constexpr auto reflected_kind_v = access::reflected_kind_v<T>;

// TODO: -cmaster Can we get rid of these functions?
//! This function template is needed to silence
//! a compiler error while the member reflectors
//! are being parsed. At that time, it doesn't
//! know where to find the on_call function,
//! or they might not even be available.
template<typename...>
void on_call(...) {}

template<typename...>
struct type_list {};

template<class R, class Derived>
using detect_is_member_type = decltype(
    on_call2(
        std::declval<R &>(),
        std::declval<Derived &>(),
        type_list<>{}
    )
);

} // namespace reflection

#define REFLECTION_REFLECTABLE_ADD_EMPTY_REFLECTOR(ReflectorClassName, ...) \
    template<class>                                                         \
    struct ReflectorClassName {}                                            \

#define REFLECTION_REFLECTABLE_ADD_MEMBER_TYPE_REFLECTOR(ReflectorClassName, member)            \
    template<class Derived>                                                                     \
    class ReflectorClassName                                                                    \
        : public reflection::reflector_base<Derived, reflection::reflected_kind::member_type>   \
    {                                                                                           \
    private:                                                                                    \
        template<typename Obj, typename... Args>                                                \
        friend auto call(ReflectorClassName &, Obj &obj, reflection::type_list<>)               \
            -> typename Obj::member;                                                            \
                                                                                                \
    public:                                                                                     \
        using member = utils::detected_or_t<                                                    \
            void,                                                                               \
            detect_is_member_type,                                                              \
            ReflectorClassName,                                                                 \
            Derived>;                                                                           \
    }                                                                                           \

// TODO: -cmaster Get rid of code duplication
#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR_NON_TEMPLATE(ReflectorClassName, member)   \
    template<typename Derived>                                                                          \
    class ReflectorClassName                                                                            \
        : public reflection::reflector_base<Derived, reflection::reflected_kind::member_function>       \
    {                                                                                                   \
    private:                                                                                            \
        template<typename Obj>                                                                          \
        friend auto call(ReflectorClassName &, Obj &obj, reflection::type_list<>)                       \
          -> decltype(obj.member())                                                                     \
        {                                                                                               \
            return obj.member();                                                                        \
        }                                                                                               \
                                                                                                        \
    public:                                                                                             \
        decltype(auto) member()                                                                         \
        {                                                                                               \
            return on_call(*this, derived(*this), reflection::type_list<>{});                           \
        }                                                                                               \
    }                                                                                                   \

// TODO: Get rid of code duplication
#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR_ASSIGNMENT_OPERATOR(ReflectorClassName, member)    \
    template<typename Derived>                                                                                  \
    class ReflectorClassName                                                                                    \
        : public reflection::reflector_base<Derived, reflection::reflected_kind::member_function>               \
    {                                                                                                           \
    private:                                                                                                    \
        template<typename Obj, typename Arg>                                                                    \
        friend auto call(ReflectorClassName &, Obj &obj, reflection::type_list<>, Arg arg)                      \
          -> decltype(obj = arg)                                                                                \
        {                                                                                                       \
            return obj = arg;                                                                                   \
        }                                                                                                       \
                                                                                                                \
    public:                                                                                                     \
        ReflectorClassName() = default;                                                                         \
        ReflectorClassName(const ReflectorClassName &) = default;                                               \
        ReflectorClassName(ReflectorClassName &&) = default;                                                    \
        ReflectorClassName &operator=(const ReflectorClassName &) = default;                                    \
        ReflectorClassName &operator=(ReflectorClassName &&) = default;                                         \
                                                                                                                \
        template<typename Arg>                                                                                  \
        decltype(auto) member(Arg &&arg)                                                                        \
        {                                                                                                       \
            return on_call(*this, derived(*this), reflection::type_list<>{}, arg);                              \
        }                                                                                                       \
    }                                                                                                           \

#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR(ReflectorClassName, member)                        \
    template<typename Derived>                                                                                  \
    class ReflectorClassName                                                                                    \
        : public reflection::reflector_base<Derived, reflection::reflected_kind::member_function>               \
    {                                                                                                           \
    private:                                                                                                    \
        template<typename Obj, typename... Args>                                                                \
        friend auto call(ReflectorClassName &, Obj &obj, reflection::type_list<>, Args &&... args)              \
          -> decltype(obj.member(std::forward<Args>(args)...))                                                  \
        {                                                                                                       \
            return obj.member(std::forward<Args>(args)...);                                                     \
        }                                                                                                       \
                                                                                                                \
        /* TODO: What if *this was an rvalue, then it should be auto &&obj */                                   \
        template<typename... ExplicitArgs, typename Obj, typename... Args>                                      \
        friend auto call(ReflectorClassName, Obj &obj, reflection::type_list<ExplicitArgs...>, Args &&... args) \
          -> std::enable_if_t<                                                                                  \
               sizeof...(ExplicitArgs) != 0,                                                                    \
               decltype(obj.template member<ExplicitArgs...>(std::forward<Args>(args)...))                      \
             >                                                                                                  \
        {                                                                                                       \
            return obj.template member<ExplicitArgs...>(std::forward<Args>(args)...);                           \
        }                                                                                                       \
                                                                                                                \
    public:                                                                                                     \
        template<typename... ExplicitArgs, typename... Args>                                                    \
        decltype(auto) member(Args &&... args)                                                                  \
        {                                                                                                       \
            return on_call(*this,                                                                               \
                           derived(*this),                                                                      \
                           reflection::type_list<ExplicitArgs...>{},                                            \
                           std::forward<Args>(args)...);                                                        \
        }                                                                                                       \
    }                                                                                                           \
