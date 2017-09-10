#pragma once

#include "reflect.h"

#include <utils/utils.h>

namespace reflection {

//! A tag-type that can be used to create generic reflections.
struct auto_;

namespace detail {

auto reflect_auto = [](auto is_auto_keyword_tester)
{
    auto fallback = [](...)
    {
        return reflect<auto_>;
    };

    return utils::make_combiner(is_auto_keyword_tester, fallback)(nullptr);
};

} // namespace detail

} // namespace reflection

#define REFLECTION_REFLECT_AUTO(Class)              \
    reflection::detail::reflect_auto(               \
        [](auto ptr, Class * cls = decltype(ptr){}) \
        {                                           \
            return reflection::reflect<             \
                std::remove_pointer_t<              \
                    decltype(cls)                   \
                >                                   \
            >;                                      \
        }                                           \
    )                                               \
