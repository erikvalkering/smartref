#pragma once

#ifndef SMARTREF_NO_DEFAULT_SUPPORT
#include "operators.h"
#include "stl.h"
#endif

#include "members.h"

#include <utils/utils.h>

namespace smartref {

template<typename Delegate, class Derived>
struct using_base
{
  operator Delegate &() &
  {
    auto &derived = static_cast<Derived &>(*this);
    return static_cast<Delegate &>(derived);
  }

  operator Delegate &&() &&
  {
    auto &&derived = static_cast<Derived &&>(*this);
    return static_cast<Delegate &&>(derived);
  }

  operator const Delegate &() const &
  {
    auto &derived = static_cast<const Derived &>(*this);
    return static_cast<const Delegate &>(derived);
  }

  operator const Delegate &&() const &&
  {
    auto &&derived = static_cast<const Derived &&>(*this);
    return static_cast<const Delegate &&>(derived);
  }
};

template<typename Delegate>
struct using_base<Delegate, void>
{
  virtual operator Delegate &() & = 0;
  virtual operator Delegate &&() && = 0;
  virtual operator const Delegate &() const & = 0;
  virtual operator const Delegate &&() const && = 0;
};

template<typename Delegate, class Derived = void>
class using_ : public using_base<Delegate, Derived>
             , public Members<
                        Delegate,
                        utils::non_void_t<Derived, using_<Delegate, Derived>>,
                        Derived,
                        using_<Delegate, Derived>
                      >
{
public:
  using_() = default;
  using_(const using_ &) = default;
  using_(using_ &&) = default;
  using_ &operator=(const using_ &) = default;
  using_ &operator=(using_ &&) = default;

  using
    Members<
      Delegate,
      utils::non_void_t<Derived, using_<Delegate, Derived>>,
      Derived,
      using_<Delegate, Derived>
    >::operator=;
};

template<typename Delegate, typename Derived>
auto delegate_type_impl(const using_<Delegate, Derived> &) -> Delegate;

template<typename Using_>
using DelegateType = decltype(delegate_type_impl(std::declval<utils::remove_cvref_t<Using_>>()));

class access
{
public:
  template<typename Using_>
  static auto delegate(Using_ &&base)
    -> utils::like_t<Using_, DelegateType<Using_>>
  {
    return static_cast<utils::like_t<Using_, DelegateType<Using_>>>(std::forward<Using_>(base));
  }
};

template<typename Using_>
auto delegate(Using_ &&base)
  SFINAEABLE_RETURN(access::delegate(std::forward<Using_>(base)))

template<typename... Hierarchy, typename Using_, typename = DelegateType<Using_>>
auto delegate_if_is_using_impl(Using_ &&base, int, int)
  SFINAEABLE_RETURN(access::delegate(std::forward<Using_>(base)))

template<typename>
struct IsUsingType : std::false_type {};

template<typename Delegate, typename Derived>
struct IsUsingType<using_<Delegate, Derived>> : std::true_type {};

template<typename>
struct DelegateType2;

template<typename Delegate, typename Derived>
struct DelegateType2<using_<Delegate, Derived>>
{
  using type = Delegate;
};

template<typename... Hierarchy, typename Using_, typename = std::enable_if_t<utils::any_of<utils::remove_cvref_t<Using_>, Hierarchy...>>>
auto delegate_if_is_using_impl(Using_ &&base, int, ...)
  -> typename DelegateType2<utils::find<IsUsingType, Hierarchy...>>::type;

template<typename... Hierarchy, typename Obj>
auto delegate_if_is_using_impl(Obj &&obj, ...)
  SFINAEABLE_RETURN(std::forward<Obj>(obj))

template<typename... Hierarchy, typename Obj>
auto delegate_if_is_using(Obj &&obj)
  SFINAEABLE_RETURN(delegate_if_is_using_impl<Hierarchy...>(std::forward<Obj>(obj), 0, 0))

template<typename, typename>
struct adl_enabler_impl;

template<typename T, size_t... slot_ids>
struct adl_enabler_impl<T, std::index_sequence<slot_ids...>>
{
  using type = utils::type_list<reflection::reflected_namespace_t<utils::Delayed<void, T>, slot_ids>...>;
};

template<typename T>
using adl_enabler = typename adl_enabler_impl<T, std::make_index_sequence<reflection::reflected_namespace_count_v<T>>>::type;

template<typename T, typename ADL = adl_enabler<T>>
struct adl_finder : using_<T, adl_finder<T, ADL>>
{
  adl_finder(T data) : data{data} {}
  operator T &() { return data; }
  T data;
};

template<typename T>
decltype(auto) enable_adl(T &&obj)
{
  return adl_finder<T>{std::forward<T>(obj)};
}

// TODO: -cmaster on_call() and call() are too similar. Come up with a different naming.
// TODO: this hook cannot be overridden if the using_<T> syntax is used,
//       which requires a runtime double dispatch mechanism.
// TODO: See if we can simplify on_call, by passing explicitargs only as template parameter

// TODO: -cmaster Document "Incomplete type support" (e.g. perfect pimpl)
template<typename ADLTag, typename Invoker, typename... Hierarchy, typename ExplicitArgs, typename Using_, typename... Args>
auto on_call(ADLTag *, const Invoker &invoker, utils::type_list<Hierarchy...>, ExplicitArgs explicitArgs, Using_ &&self, Args &&... args)
  SFINAEABLE_RETURN(
    call(
      invoker,
      explicitArgs,
      delegate_if_is_using<Hierarchy...>(std::forward<Using_>(self)),
      delegate_if_is_using<Hierarchy...>(std::forward<Args>(args))...
    )
  )

} // namespace smartref
