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

template<class Derived>
class reflector_base
{
private:
    template<typename Self>
    friend auto derived(Self &self)
      -> utils::Delayed<Derived, Self> &
    {
        return static_cast<Derived &>(self);
    }
};

template<class Reflection, class Derived>
using detect_is_member_type = decltype(
    on_call(
        std::declval<Reflection &>(),
        std::declval<Derived &>(),
        utils::type_list<>{}
    )
);

} // namespace reflection

#define REFLECTION_REFLECTABLE_ADD_EMPTY_REFLECTOR(ReflectorClassName, ...) \
    template<class>                                                         \
    struct ReflectorClassName {}                                            \

#define REFLECTION_REFLECTABLE_ADD_MEMBER_TYPE_REFLECTOR(ReflectorClassName, member)    \
    template<class Derived>                                                             \
    class ReflectorClassName                                                            \
        : public reflection::reflector_base<Derived>                                    \
    {                                                                                   \
    private:                                                                            \
        template<typename Obj, typename... Args>                                        \
        friend auto call(ReflectorClassName &, Obj &obj, utils::type_list<>)       \
            -> typename Obj::member;                                                    \
                                                                                        \
    public:                                                                             \
        using member = utils::detected_or_t<                                            \
            void,                                                                       \
            detect_is_member_type,                                                      \
            ReflectorClassName,                                                         \
            Derived>;                                                                   \
    }                                                                                   \

// TODO: -cmaster Get rid of code duplication
#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR_NON_TEMPLATE(ReflectorClassName, member)   \
    template<typename Derived>                                                                          \
    class ReflectorClassName                                                                            \
        : public reflection::reflector_base<Derived>                                                    \
    {                                                                                                   \
    private:                                                                                            \
        template<typename Obj>                                                                          \
        friend auto call(ReflectorClassName &, Obj &obj, utils::type_list<>)                       \
          -> decltype(obj.member())                                                                     \
        {                                                                                               \
            return obj.member();                                                                        \
        }                                                                                               \
                                                                                                        \
    public:                                                                                             \
        auto member()                                                                                   \
          -> decltype(on_call(*this, derived(*this), utils::type_list<>{}))                        \
        {                                                                                               \
            return on_call(*this, derived(*this), utils::type_list<>{});                           \
        }                                                                                               \
    }                                                                                                   \

// TODO: Get rid of code duplication
#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR_ASSIGNMENT_OPERATOR(ReflectorClassName, member)    \
    template<typename Derived>                                                                                  \
    class ReflectorClassName                                                                                    \
        : public reflection::reflector_base<Derived>                                                            \
    {                                                                                                           \
    private:                                                                                                    \
        template<typename Obj, typename Arg>                                                                    \
        friend auto call(ReflectorClassName &, Obj &obj, utils::type_list<>, Arg arg)                      \
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
        auto member(Arg &&arg)                                                                                  \
          -> decltype(                                                                                          \
            on_call(                                                                                            \
                *this,                                                                                          \
                derived(utils::delayed(*this, utils::type_list<Arg>{})),                                   \
                utils::type_list<>{}, arg                                                                  \
            )                                                                                                   \
          )                                                                                                     \
        {                                                                                                       \
            return on_call(                                                                                     \
                *this,                                                                                          \
                derived(utils::delayed(*this, utils::type_list<Arg>{})),                                   \
                utils::type_list<>{}, arg                                                                  \
            );                                                                                                  \
        }                                                                                                       \
    }                                                                                                           \

#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR(ReflectorClassName, member)                        \
    template<typename Derived>                                                                                  \
    class ReflectorClassName                                                                                    \
        : public reflection::reflector_base<Derived>                                                            \
    {                                                                                                           \
    private:                                                                                                    \
        template<typename Obj, typename... Args>                                                                \
        friend auto call(ReflectorClassName &, Obj &obj, utils::type_list<>, Args &&... args)              \
          -> decltype(obj.member(std::forward<Args>(args)...))                                                  \
        {                                                                                                       \
            return obj.member(std::forward<Args>(args)...);                                                     \
        }                                                                                                       \
                                                                                                                \
        /* TODO: What if *this was an rvalue, then it should be auto &&obj */                                   \
        template<typename... ExplicitArgs, typename Obj, typename... Args>                                      \
        friend auto call(ReflectorClassName, Obj &obj, utils::type_list<ExplicitArgs...>, Args &&... args) \
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
        auto member(Args &&... args)                                                                            \
          -> decltype(                                                                                          \
            on_call(*this,                                                                                      \
                    derived(utils::delayed(*this, utils::type_list<ExplicitArgs...>{})),                   \
                    utils::type_list<ExplicitArgs...>{},                                                   \
                    std::forward<Args>(args)...)                                                                \
          )                                                                                                     \
        {                                                                                                       \
            return on_call(*this,                                                                               \
                           derived(utils::delayed(*this, utils::type_list<ExplicitArgs...>{})),            \
                           utils::type_list<ExplicitArgs...>{},                                            \
                           std::forward<Args>(args)...);                                                        \
        }                                                                                                       \
    }                                                                                                           \
