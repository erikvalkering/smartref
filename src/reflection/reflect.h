#pragma once

#include "../reflection_intrusive.h"
#include "../reflection_nonintrusive.h"

#include <tuple>

namespace reflection {

namespace detail {

template<typename Class, typename index_pack_intrusive, typename index_pack_non_intrusive>
struct Reflection;

template<typename Class, size_t... indices_intrusive, size_t... indices_non_intrusive>
struct Reflection<Class, std::index_sequence<indices_intrusive...>, std::index_sequence<indices_non_intrusive...>>
{
    auto members() const
    {
        return std::tuple<
            reflected_member_intrusive_t<Class, indices_intrusive>...,
            reflected_member_t<Class, indices_non_intrusive>...
        >{};
    }
};

} // namespace detail

template<typename Class>
using Reflection = detail::Reflection<
    Class,
    std::make_index_sequence<reflected_member_intrusive_count_v<Class>>,
    std::make_index_sequence<reflected_member_count_v<Class>>
>;

template<typename Class>
constexpr auto reflect = Reflection<Class>{};

} // namespace reflection
