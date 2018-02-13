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

protected:
    decltype(auto) derived()
    {
        return static_cast<Derived &>(*this);
    }
};

template<typename T>
constexpr auto reflected_kind_v = access::reflected_kind_v<T>;

} // namespace reflection

#define REFLECTION_REFLECTABLE_ADD_MEMBER_TYPE_REFLECTOR(ReflectorClassName, member)            \
    template<typename Class>                                                                    \
    class ReflectorClassName                                                                    \
        : public reflection::reflector_base<Derived, reflection::reflected_kind::member_type>   \
    {                                                                                           \
    public:                                                                                     \
        using member = typename Class::member;                                                  \
    };                                                                                          \
                                                                                                \
    template<typename Class>                                                                    \
    using detect_is_member_type = decltype(                                                     \
        std::declval<typename Class::member>())                                                 \

// TODO: Get rid of code duplication
#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR_NON_TEMPLATE(ReflectorClassName, member)   \
    template<typename Derived>                                                                          \
    class ReflectorClassName                                                                            \
        : public reflection::reflector_base<Derived, reflection::reflected_kind::member_function>       \
    {                                                                                                   \
    private:                                                                                            \
        template<typename Obj>                                                                          \
        friend decltype(auto) call(ReflectorClassName, Obj &obj)                                        \
        {                                                                                               \
            return obj.member();                                                                        \
        }                                                                                               \
                                                                                                        \
    public:                                                                                             \
        decltype(auto) member()                                                                         \
        {                                                                                               \
            return on_call(*this, this->derived());                                                     \
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
        friend decltype(auto) call(ReflectorClassName, Obj &obj, Arg arg)                                       \
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
            /* TODO: For some unknown reason, *not* prefixing the call to derived */                            \
            /*       with 'this->', will result in the member-function, which is  */                            \
            /*       declared in the base-class, to be hidden.                    */                            \
            return on_call(*this, this->derived(), arg);                                                        \
        }                                                                                                       \
    }                                                                                                           \

#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR(ReflectorClassName, member)            \
    template<typename Derived>                                                                      \
    class ReflectorClassName                                                                        \
        : public reflection::reflector_base<Derived, reflection::reflected_kind::member_function>   \
    {                                                                                               \
    private:                                                                                        \
        template<typename Obj, typename... Args>                                                    \
        friend decltype(auto) call(ReflectorClassName, Obj &obj, Args &&... args)                   \
        {                                                                                           \
            return obj.member(std::forward<Args>(args)...);                                         \
        }                                                                                           \
                                                                                                    \
        /* TODO: What if *this was an rvalue, then it should be auto &&obj */                       \
        template<typename ExplicitArgs..., typename Obj, typename... Args>                          \
        friend decltype(auto) call(ReflectorClassName, Obj &obj, Args &&... args)                   \
        {                                                                                           \
            return obj.template member<ExplicitArgs...>(std::forward<Args>(args)...);               \
        }                                                                                           \
                                                                                                    \
    public:                                                                                         \
        template<typename... ExplicitArgs, typename... Args>                                        \
        decltype(auto) member(Args &&... args)                                                      \
        {                                                                                           \
            return on_call<ExplicitArgs...>(*this,                                                  \
                                            this->derived(),                                        \
                                            std::forward<Args>(args)...);                           \
        }                                                                                           \
    }                                                                                               \
