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

    template<typename Derived>
    class reflect_member_function
        : public reflection::reflector_base<Derived, reflection::reflected_kind::member_function>
    {
    private:
        template<typename Obj, typename Arg>
        friend auto call(reflect_member_function, Obj &obj, Arg arg)
            -> decltype(obj = arg)
        {
            return obj = arg;
        }

    public:
        reflect_member_function() = default;
        reflect_member_function(const reflect_member_function &) = default;
        reflect_member_function(reflect_member_function &&) = default;
        reflect_member_function &operator=(const reflect_member_function &) = default;
        reflect_member_function &operator=(reflect_member_function &&) = default;

        template<typename Arg>
        decltype(auto) operator=(Arg &&arg)
        {
            // TODO: For some unknown reason, *not* prefixing the call to derived
            //       with 'this->', will result in the member-function, which is
            //       declared in the base-class, to be hidden.
            return on_call(*this, this->derived(), arg);
        }
    };

// TODO: Unify the non-generic and generic ReflectedMembers - how they are registered is kind of an 'implementation detail'
template<typename Delegate, class Derived = void>
class using_ : public using_base<Delegate, Derived>
             // , public Members<Delegate, using_<Delegate, Derived>>
             // , public ReflectedMembers<Delegate, using_<Delegate, Derived>>
             // , public ReflectedMembers<Delegate, using_<Delegate, Derived>, utils::Delayed<reflection::auto_, Delegate>>
             , public reflect_member_function<Derived>
{
public:
    using reflect_member_function<Derived>::operator=;

    using_() = default;

    using_(const using_ &) = default;
    using_ &operator=(const using_ &) = default;

    using_(using_ &&) = default;
    using_ &operator=(using_ &&) = default;

    // using ReflectedMembers<Delegate, using_<Delegate, Derived>>::operator=...;
    // using ReflectedMembers<Delegate, using_<Delegate, Derived>, utils::Delayed<reflection::auto_, Delegate>>::operator=...;
};

// TODO: on_call() and call() are too similar. Come up with a different naming.
template<typename Reflection, typename Delegate, typename Derived, typename Args>
auto on_call(Reflection reflection, using_<Delegate, Derived> &self, Args args)
  -> decltype(call(reflection, static_cast<Delegate &>(self), std::forward<Args>(args)))
{
  return call(reflection, static_cast<Delegate &>(self), std::forward<Args>(args));
}

} // namespace smartref
