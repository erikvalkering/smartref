#pragma once

#include "members.h"
#include "stl.h"

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
             , public Members<Delegate, utils::non_void_t<Derived, using_<Delegate, Derived>>>
{
public:
  using Members<Delegate, utils::non_void_t<Derived, using_<Delegate, Derived>>>::operator=;

  using_() = default;

  using_(const using_ &) = default;
  using_ &operator=(const using_ &) = default;

  using_(using_ &&) = default;
  using_ &operator=(using_ &&) = default;
};

template<typename>
struct DelegateTypeImpl;

template<typename Delegate, typename Derived>
struct DelegateTypeImpl<using_<Delegate, Derived>>
{
  using type = Delegate;
};

template<typename Using_>
using DelegateType = typename DelegateTypeImpl<utils::remove_cvref_t<Using_>>::type;

template<typename Using_>
auto delegate(Using_ &&base)
  -> utils::like_t<Using_, DelegateType<Using_>>
{
  return static_cast<utils::like_t<Using_, DelegateType<Using_>>>(std::forward<Using_>(base));
}

// TODO: -cmaster on_call() and call() are too similar. Come up with a different naming.
// TODO: this hook cannot be overridden if the using_<T> syntax is used,
//       which requires a runtime double dispatch mechanism.

// TODO: -cmaster Document "Incomplete type support" (e.g. perfect pimpl)
// TODO: Maybe pass ExplicitArgs... as-is (i.e. not as a parameter pack, but single type_list parameter)
template<typename Invoker, typename... ExplicitArgs, typename Using_, typename... Args>
auto on_call(const Invoker &invoker, utils::type_list<ExplicitArgs...> explicitArgs, Using_ &&self, Args &&... args)
  SFINAEABLE_RETURN(
    call(invoker, explicitArgs, delegate(std::forward<Using_>(self)), std::forward<Args>(args)...)
  )

} // namespace smartref
