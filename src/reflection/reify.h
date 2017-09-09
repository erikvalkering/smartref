#pragma once

#include "../utils.h"

namespace reflection {

namespace detail {

template<typename Reflection, typename F>
using detect_is_member_type = typename Reflection::template detect_is_member_type<F>;

template<class Reflection, typename F>
constexpr static auto is_member_type()
{
    return utils::is_detected_v<detect_is_member_type, Reflection, F>;
}

template<typename... Ts>
constexpr auto always_true = true;

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

} // namespace detail

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
