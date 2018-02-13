#pragma once

#include "explicit.h"
#include "implicit.h"
#include "stl.h"

#include <utils/utils.h>

namespace smartref {

template<typename Delegate, class Derived>
struct using_base
{
    operator Delegate &()
    {
        auto &derived = static_cast<Derived &>(*this);
        return static_cast<Delegate &>(derived);
    }
};

template<typename Delegate>
struct using_base<Delegate, void>
{
    // TODO: Add some checks to see whether this doesn't add too much overhead
    //       for this specific Delegate type.
    virtual operator Delegate &() = 0;
};

template<typename Derived, typename Fallback>
struct non_void
{
    using type = Derived;
};

template<typename Fallback>
struct non_void<void, Fallback>
{
    using type = Fallback;
};

template<typename Derived, typename Fallback>
using non_void_t = typename non_void<Derived, Fallback>::type;

// TODO: Unify the non-generic and generic ReflectedMembers - how they are registered is kind of an 'implementation detail'
template<typename Delegate, class Derived = void>
class using_ : public using_base<Delegate, Derived>
             // , public Members<Delegate, non_void_t<Derived, using_<Delegate, Derived>>>
             , public ReflectedMembers<Delegate, non_void_t<Derived, using_<Delegate, Derived>>>
             , public ReflectedMembers<Delegate, non_void_t<Derived, using_<Delegate, Derived>>, utils::Delayed<reflection::auto_, Delegate>>
{
public:
    using ReflectedMembers<Delegate, non_void_t<Derived, using_<Delegate, Derived>>, utils::Delayed<reflection::auto_, Delegate>>::operator=;

    using_() = default;

    using_(const using_ &) = default;
    using_ &operator=(const using_ &) = default;

    using_(using_ &&) = default;
    using_ &operator=(using_ &&) = default;
};

// TODO: on_call() and call() are too similar. Come up with a different naming.
// TODO: this hook cannot be overridden if the using_<T> syntax is used,
//       which requires a runtime double dispatch mechanism.
// TODO: Fix ExplicitArgs
// TODO: Reflection is not the actual member reflection, but the reflector
//       (i.e. the class from which we inherit the member-function)
template<typename Reflection, typename Delegate, typename Derived, typename... Args>
auto on_call(Reflection reflection, using_<Delegate, Derived> &self, Args... args)
  -> decltype(call(reflection, static_cast<Delegate &>(self), std::forward<Args>(args)...))
{
  return call(reflection, static_cast<Delegate &>(self), std::forward<Args>(args)...);
}

} // namespace smartref
