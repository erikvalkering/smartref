#pragma once

namespace reflection {

template<typename Class>
struct Reflection
{
  constexpr auto operator==(Reflection) const { return true; }
  constexpr auto operator!=(Reflection) const { return false; }

  template<typename U>
  constexpr auto operator==(U) const { return false; }
  template<typename U>
  constexpr auto operator!=(U) const { return true; }
};

template<typename Class>
constexpr auto reflect = Reflection<Class>{};

} // namespace reflection
