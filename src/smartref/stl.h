#pragma once

#include "explicit.h"

namespace smartref {

DECLARE_USING_MEMBER_TYPE(value_type);
DECLARE_USING_MEMBER_TYPE(difference_type);
DECLARE_USING_MEMBER_TYPE(iterator);

template<typename Delegate, class Derived>
struct STL : USING_MEMBER_TYPE(Delegate, value_type)
           , USING_MEMBER_TYPE(Delegate, difference_type)
           , USING_MEMBER_TYPE(Delegate, iterator)
{
    USING_MEMBER(push_back)
    USING_MEMBER(begin)
    USING_MEMBER(end)
};

} // namespace smartref
