#pragma once

#include "reflect.h"
#include "reflectors.h"

#include <utils/utils.h>

namespace reflection {

namespace detail {

template<class Reflection, typename Derived, typename reflector>
constexpr static auto is_member_type_impl(std::true_type)
{
  return utils::is_detected_v<
    reflection::detect_is_member_type,
    reflection::Invoker<reflector>,
    Derived
  >;
}

template<typename...>
constexpr static auto is_member_type_impl(std::false_type)
{
  return false;
}

template<class Reflection, typename Derived>
constexpr static auto is_member_type()
{
  using reflector = typename Reflection::template reflector_member_type<Derived>;

  auto tag = std::integral_constant<bool, is_reflector(reflector{})>{};

  return is_member_type_impl<Reflection, Derived, reflector>(tag);
}

template<class Reflection>
constexpr static auto is_member_function()
{
  //! Member-functions currently cannot be detected (yet).
  //! However, that is not a problem, because they will be SFINAE'ed away,
  //! in case the reflected entity wasn't a member-function.
  // TODO: It might still be an annoyance for IDEs, though, where it could show
  //       the candidate member-functions, even though none exists.
  // TODO: -cmaster Come up with a mechanism to reject member functions before calling them,
  //       by checking for existence of a member that is not a member type (nor a data member).
  //       struct Detector : T, BAR
  //       but will not work for final classes
  return utils::always_true<Reflection>;
}

} // namespace detail

template<typename T>
constexpr static auto reify(Reflection<T>) -> T;

template<class Derived, class Reflection>
constexpr static auto reify_members(Reflection refl)
{
  if constexpr (detail::is_member_type<Reflection, Derived>())
  {
    return typename Reflection::template reflector_member_type<Derived>{};
  }
  else if constexpr (detail::is_member_function<Reflection>())
  {
    return typename Reflection::template reflector_member_function<Derived>{};
  }
}

template<class Derived, class Reflection>
constexpr static auto reify(Reflection refl)
{
  return utils::Compose<
    decltype(reify_members<Derived>(refl)),
    typename Reflection::template reflector_free_function<Derived>
  >{};
}

} // namespace reflection
