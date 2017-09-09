#pragma once

#include "../reflection_intrusive.h"
#include "../reflection_nonintrusive.h"
#include "reflect.h"

#include <tuple>

namespace reflection {

namespace detail {

template<typename Class, typename index_pack_intrusive, typename index_pack_non_intrusive>
struct Members;

template<typename Class, size_t... indices_intrusive, size_t... indices_non_intrusive>
struct Members<Class, std::index_sequence<indices_intrusive...>, std::index_sequence<indices_non_intrusive...>>
{
    constexpr static auto _()
    {
        return std::tuple<
            reflected_member_intrusive_t<Class, indices_intrusive>...,
            reflected_member_t<Class, indices_non_intrusive>...
        >{};
    }
};

} // namespace detail

template<typename Class>
constexpr auto members(Reflection<Class>)
{
    return detail::Members<
        Class,
        std::make_index_sequence<reflected_member_intrusive_count_v<Class>>,
        std::make_index_sequence<reflected_member_count_v<Class>>
    >::_();
}

} // namespace reflection
