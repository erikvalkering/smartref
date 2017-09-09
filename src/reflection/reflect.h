#pragma once

namespace reflection {

template<typename Class>
struct Reflection {};

template<typename Class>
constexpr auto reflect = Reflection<Class>{};

} // namespace reflection
