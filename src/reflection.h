#pragma once

#include "reflection/reflect.h"
#include "utils.h"
#include <tuple>

namespace reflection {

template<typename... Ts>
constexpr auto always_true = true;

template<typename Reflection, typename F>
using detect_is_member_type = typename Reflection::template detect_is_member_type<F>;

template<class Reflection, typename F>
constexpr static auto is_member_type()
{
    return utils::is_detected_v<detect_is_member_type, Reflection, F>;
}

template<class Reflection>
constexpr static auto is_member_function()
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
    // TODO: Shouldn't we be passing the class instead of F?
    if constexpr (is_member_type<Reflection, F>())
    {
        return typename Reflection::template reflect_member_type<F>{};
    }
    else if constexpr (is_member_function<Reflection>())
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
