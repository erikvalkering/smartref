#pragma once

#include "reflection_common.h"
#include "counter.h"

namespace reflection {

// TODO: Rename to intrusive and non-intrusive

template<typename T, size_t counter>
struct reflected_member
{
    using type = void;
};

template<typename T, size_t counter>
using reflected_member_t = typename reflected_member<T, counter>::type;

template<typename T>
constexpr auto reflected_member_count_v = reflected_member_count<reflected_member_t, T>::value;

} // namespace reflection

#define REFLECTION_REFLECT_NONINTRUSIVE(Class, member)                  \
    template<>                                                          \
    struct reflection::reflected_member<Class, CURRENT_COUNTER(Class)>  \
    {                                                                   \
        using type = struct                                             \
        {                                                               \
            REFLECTION_REFLECT_ADD_MEMBER_TYPE_REFLECTOR(               \
                reflect_member_type,                                    \
                Class,                                                  \
                member);                                                \
                                                                        \
            REFLECTION_REFLECT_ADD_MEMBER_FUNCTION_REFLECTOR(           \
                reflect_member_function,                                \
                member);                                                \
        };                                                              \
    };                                                                  \
                                                                        \
    INC_COUNTER(Class)                                                  \
