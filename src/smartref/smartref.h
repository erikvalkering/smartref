#pragma once

// TODO: -cmaster remove explicit.h
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

// TODO: -cmaster rename the non_void stuff. And maybe it can go to utils?
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
             , public Members<Delegate, non_void_t<Derived, using_<Delegate, Derived>>>
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

// TODO: -cmaster on_call() and call() are too similar. Come up with a different naming.
// TODO: this hook cannot be overridden if the using_<T> syntax is used,
//       which requires a runtime double dispatch mechanism.
// TODO: Reflection is not the actual member reflection, but the reflector
//       (i.e. the class from which we inherit the member-function)

//! This function template is needed to silence
//! a compiler error while the on_call function
//! template below is being parsed. At that time,
//! it doesn't know where to find the call function,
//! or it might not be available at all.
template<typename...>
void call(...) {}

// TODO: -cmaster Reflection should be named Reflector (better: rename everything)
// TODO: -cmaster Instead of passing the reflector, pass a Reflection, such that we can also reify that directly
// TODO: -cmaster args should use forwarding references (unit test this!)
template<typename... ExplicitArgs, typename Reflection, typename Delegate, typename Derived, typename... Args>
auto on_call(Reflection &reflection, using_<Delegate, Derived> &self, reflection::type_list<ExplicitArgs...> explicitArgs, Args... args)
  -> decltype(call(reflection, delegate(self), explicitArgs, std::forward<Args>(args)...))
{
  return call(reflection, delegate(self), explicitArgs, std::forward<Args>(args)...);
}

template<typename Delegate, typename Derived>
auto delegate(using_<Delegate, Derived> &base)
  -> Delegate &
{
    return static_cast<Delegate &>(base);
}

// TODO: -cmaster Maybe it's better to make Reflection *only* a template parameter
// TODO: -cmaster Document "Incomplete type support" (e.g. perfect pimpl)
template<typename Reflection, typename Delegate, typename Derived, typename... ExplicitArgs, typename... Args>
auto on_call2(Reflection &reflection, using_<Delegate, Derived> &self, reflection::type_list<ExplicitArgs...> explicitArgs = reflection::type_list<>{}, Args... args)
  -> decltype(call2(reflection, delegate(self), explicitArgs, std::forward<Args>(args)...))
{
  return call2(reflection, delegate(self), explicitArgs, std::forward<Args>(args)...);
}

} // namespace smartref
