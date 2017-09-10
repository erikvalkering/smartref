#pragma once

#include "reflect.h"

#include <utils/utils.h>

namespace reflection {

namespace detail {

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
    return utils::always_true<Reflection>;
}

} // namespace detail

template<typename T>
constexpr static auto reify(Reflection<T>) -> T;

template<class Reflection, typename F>
constexpr static auto reify(Reflection refl, F)
{
    // TODO: Shouldn't we be passing the class instead of F?
    if constexpr (detail::is_member_type<Reflection, F>())
    {
        return typename Reflection::template reflect_member_type<F>{};
    }
    else if constexpr (detail::is_member_function<Reflection>())
    {
        return typename Reflection::template reflect_member_function<F>{};
    }
}

} // namespace reflection
