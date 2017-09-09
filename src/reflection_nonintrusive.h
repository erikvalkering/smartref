#pragma once

#include "counter.h"
#include "reflection/reflect_auto.h"
#include "reflection/reify.h"
#include "reflection_common.h"

namespace reflection {

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

#define REFLECTION_REFLECT_NONINTRUSIVE(Class, member)          \
    constexpr auto CONCAT(reflection, __LINE__) =               \
        REFLECTION_REFLECT_AUTO(Class);                         \
                                                                \
    using CONCAT(CLASS, __LINE__) =                             \
        decltype(reify(CONCAT(reflection, __LINE__)));          \
                                                                \
    template<>                                                  \
    struct reflection::reflected_member<                        \
        CONCAT(CLASS, __LINE__),                                \
        CURRENT_COUNTER(CONCAT(CLASS, __LINE__))                \
    >                                                           \
    {                                                           \
        using type = struct                                     \
        {                                                       \
            REFLECTION_REFLECT_ADD_MEMBER_TYPE_REFLECTOR(       \
                reflect_member_type,                            \
                member);                                        \
                                                                \
            REFLECTION_REFLECT_ADD_MEMBER_FUNCTION_REFLECTOR(   \
                reflect_member_function,                        \
                member);                                        \
        };                                                      \
    };                                                          \
                                                                \
    INC_COUNTER(CONCAT(CLASS, __LINE__))                        \
