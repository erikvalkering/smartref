#pragma once

#include "counter.h"
#include "reflection/reflect.h"
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

namespace detail {

auto safe_reflect = [](auto is_auto_keyword_tester)
{
    auto fallback = [](...)
    {
        return reflect<auto_>;
    };

    return utils::make_combiner(is_auto_keyword_tester, fallback)(nullptr);
};

} // namespace detail

} // namespace reflection

#define REFLECTION_SAFE_REFLECT(Class)              \
    reflection::detail::safe_reflect(               \
        [](auto ptr, Class * cls = decltype(ptr){}) \
        {                                           \
            return reflection::reflect<             \
                std::remove_pointer_t<              \
                    decltype(cls)                   \
                >                                   \
            >;                                      \
        }                                           \
    )                                               \

#define REFLECTION_REFLECT_NONINTRUSIVE(Class, member)          \
    constexpr auto CONCAT(reflection, __LINE__) =               \
        REFLECTION_SAFE_REFLECT(Class);                         \
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
