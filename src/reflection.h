#pragma once

#include "reflection_intrusive.h"
#include "reflection_nonintrusive.h"
#include "utils.h"

#include <tuple>

namespace reflection {

template<typename... Ts>
constexpr auto always_true = true;

namespace detail {

template<typename Class, typename index_pack_intrusive, typename index_pack_non_intrusive>
struct Reflection;

template<typename Class, size_t... indices_intrusive, size_t... indices_non_intrusive>
struct Reflection<Class, std::index_sequence<indices_intrusive...>, std::index_sequence<indices_non_intrusive...>>
{
    auto members() const
    {
        return std::tuple<
            reflected_class_member_t<Class, indices_intrusive>...,
            reflected_member_t<Class, indices_non_intrusive>...
        >{};
    }
};

} // namespace detail

template<typename Class>
using Reflection = detail::Reflection<
    Class,
    std::make_index_sequence<reflected_class_member_count_v<Class>>,
    std::make_index_sequence<reflected_member_count_v<Class>>
>;

template<typename Class>
constexpr auto reflect = Reflection<Class>{};

template<typename Reflection>
using detect_is_member_type = typename Reflection::template detect_is_member_type<void>;

template<class Reflection>
constexpr static auto is_member_type(Reflection)
{
    return utils::is_detected_v<detect_is_member_type, Reflection>;
}

template<class Reflection>
constexpr static auto is_member_function(Reflection)
{
    //! Member-functions currently cannot be detected (yet).
    //! However, that is not a problem, because they will be SFINAE'ed away,
    //! in case the reflected entity wasn't a member-function.
    // TODO: It might still be an annoyance for IDEs, though, where it could show
    //       the candidate member-functions, even though none exists.
    return always_true<Reflection>;
}

template<class Reflection, typename F>
constexpr static auto reify(Reflection refl, F)
{
    if constexpr (is_member_type(refl))
    {
        return typename Reflection::template reflect_member_type<F>{};
    }
    else if constexpr (is_member_function(refl))
    {
        return typename Reflection::template reflect_member_function<F>{};
    }
}

} // namespace reflection

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
