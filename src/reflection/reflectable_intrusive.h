#pragma once

#include "reflectable_common.h"

#include <utils/counter.h>
#include <utils/utils.h>

namespace reflection {

template<typename T, size_t counter, typename = void>
struct reflected_member_intrusive
{
    using type = void;
};

// TODO: unify non-intrusive to use same mechanism
template<typename T, size_t counter>
struct reflected_member_intrusive<T, counter, std::enable_if_t<decltype(std::declval<T>().__reflect(counter::Counter<counter>{}))::value == counter>>
{
    using type = decltype(std::declval<T>().__reflect(counter::Counter<counter>{}));
};

template<typename T, size_t counter>
using reflected_member_intrusive_t = typename reflected_member_intrusive<T, counter>::type;

template<typename T>
constexpr auto reflected_member_intrusive_count_v = reflected_member_count<reflected_member_intrusive_t, T>::value;

} // namespace reflection

// TODO: REFLECTABLE currently doesn't support member-functions declared using 'auto' --> workaround: REFLECTION_REFLECTABLE_NONINTRUSIVE
// TODO: REFLECTABLE currently doesn't support member-functions templates --> workaround: REFLECTION_REFLECTABLE_NONINTRUSIVE
// TODO: REFLECTABLE currently doesn't support member-functions declared using 'virtual' --> workaround: REFLECTION_REFLECTABLE_NONINTRUSIVE
#define REFLECTION_REFLECTABLE_INTRUSIVE_IMPL(member)                   \
    REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR(               \
        __reflect__##member,                                            \
        member);                                                        \
                                                                        \
    template<typename Counter, class Class>                             \
    struct __type##member : Counter                                     \
    {                                                                   \
        template<typename F>                                            \
        using reflect_member_function = __reflect__##member<F>;         \
    };                                                                  \
                                                                        \
    auto __reflect(counter::Counter<CURRENT_CLASS_COUNTER()> counter)   \
    {                                                                   \
        using Class = std::decay_t<decltype(*this)>;                    \
                                                                        \
        using type = __type##member<decltype(counter), Class>;          \
                                                                        \
        return type{};                                                  \
    }                                                                   \
                                                                        \
    INC_CLASS_COUNTER()                                                 \

#define INJECT_CODE_MEMBER_FUNCTION(member, injection_function)                                         \
    __injection_return_type_##member() {}                                                               \
                                                                                                        \
    injection_function(member);                                                                         \
                                                                                                        \
    /* Here, we basically replace the original member-function,                                     */  \
    /* by defining a new member-function template with the same name,                               */  \
    /* which simply forwards to the implementation of the original member-function.                 */  \
    template<typename... Args>                                                                          \
    auto member(Args &&... args) -> decltype(__injection_return_type_##member())                        \
    {                                                                                                   \
        /* Because the function containing the implementation is defined                            */  \
        /* after this member function, it is not available at this point.                           */  \
        /* By making the evaluation of '*this' dependent on the template arguments,                 */  \
        /* we can work around this (this is exactly what 'delayed()' does).                         */  \
        return utils::delayed<Args...>(*this).__injection_impl_##member(std::forward<Args>(args)...);   \
    }                                                                                                   \
                                                                                                        \
    /* Here, we define a member-function that will contain the implementation                       */  \
    /* of the original member-function.                                                             */  \
    auto __injection_impl_##member                                                                      \

#define REFLECTION_REFLECTABLE_INTRUSIVE(member)                                \
    INJECT_CODE_MEMBER_FUNCTION(member, REFLECTION_REFLECTABLE_INTRUSIVE_IMPL)  \
