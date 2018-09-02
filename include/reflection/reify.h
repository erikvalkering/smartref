#pragma once

#include "reflect.h"
#include "reflectors.h"

#include <utils/utils.h>

namespace reflection {

namespace detail {

template<class Reflection, typename Derived, typename reflector, typename... Hierarchy>
constexpr auto is_member_type_impl(std::true_type)
{
  return utils::is_detected_v<
    reflection::detect_is_member_type,
    reflection::Invoker<reflector>,
    Derived,
    Hierarchy...
  >;
}

template<typename...>
constexpr auto is_member_type_impl(std::false_type)
{
  return false;
}

template<class Reflection, typename Derived, typename... Hierarchy>
constexpr auto is_member_type()
{
  using reflector = typename Reflection::template reflector_member_type<Derived>;

  auto tag = std::integral_constant<bool, is_reflector(reflector{})>{};

  return is_member_type_impl<Reflection, Derived, reflector, Hierarchy...>(tag);
}

template<class Reflection, typename... Hierarchy>
constexpr auto is_member_function()
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
constexpr auto reify(Reflection<T>) -> T;

template<class Derived, typename... Hierarchy, class Reflection>
constexpr auto reify_members(Reflection refl)
{
  if constexpr (detail::is_member_type<Reflection, Derived, Hierarchy...>())
  {
    return typename Reflection::template reflector_member_type<Derived, Hierarchy...>{};
  }
  else if constexpr (detail::is_member_function<Reflection>())
  {
    return typename Reflection::template reflector_member_function<Derived, Hierarchy...>{};
  }
}

template<class Derived, class Reflection, typename... Hierarchy>
struct CreateMemberBaseConstructor
{
  template<typename CRTP>
  using BaseConstructor = decltype(reify_members<Derived, CRTP, Hierarchy...>(Reflection{}));

  using type = utils::metafunction<BaseConstructor>;
};

template<class Derived, class Reflection, typename... Hierarchy>
struct CreateFreeFunctionBaseConstructor
{
  template<typename CRTP>
  using BaseConstructor = typename Reflection::template reflector_free_function<Derived, CRTP, Hierarchy...>;

  using type = utils::metafunction<BaseConstructor>;
};

template<class Derived, typename... Hierarchy, class Reflection>
constexpr auto reify(Reflection refl)
{
  return
    utils::ClassConstructor<
      typename CreateMemberBaseConstructor<Derived, Reflection, Hierarchy...>::type,
      typename CreateFreeFunctionBaseConstructor<Derived, Reflection, Hierarchy...>::type
    >{};
}

} // namespace reflection
