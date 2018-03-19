#pragma once

#include "reflect.h"
#include "reflect_auto.h"
#include "reflectable_nonintrusive.h"

#include "utils/utils.h"

namespace reflection {

namespace detail {

template<typename Class, typename index_pack_non_intrusive, typename index_pack_non_intrusive_auto>
struct Members;

template<typename Class, size_t... indices_non_intrusive, size_t... indices_nonintrusive_auto>
struct Members<Class, std::index_sequence<indices_non_intrusive...>, std::index_sequence<indices_nonintrusive_auto...>>
{
    constexpr static auto _()
    {
        return utils::type_list<
            reflected_member_t<Class, indices_non_intrusive>...,
            reflected_member_t<utils::Delayed<auto_, Class>, indices_nonintrusive_auto>...
        >{};
    }
};

} // namespace detail

template<typename Class>
constexpr auto members(Reflection<Class>)
{
    return detail::Members<
        Class,
        std::make_index_sequence<reflected_member_count_v<Class>>,
        std::make_index_sequence<reflected_member_count_v<utils::Delayed<auto_, Class>>>
    >::_();
}

} // namespace reflection
