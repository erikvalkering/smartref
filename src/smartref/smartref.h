#pragma once

#include "explicit.h"
#include "implicit.h"
#include "stl.h"

#include <utils/utils.h>

namespace smartref {

template<typename Delegate, class Derived = void>
class using_;

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

template<typename Delegate, class Derived>
class using_ : public using_base<Delegate, Derived>
             , public Members<Delegate, using_<Delegate, Derived>>
             , public ReflectedMembers<Delegate, using_<Delegate, Derived>>
             , public ReflectedMembers<Delegate, using_<Delegate, Derived>, utils::Delayed<reflection::auto_, Delegate>>
{
};

} // namespace smartref
