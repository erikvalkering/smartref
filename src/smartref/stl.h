#pragma once

#include <reflection/reflection.h>

// TODO: Move reflection stuff not specific to STL out of this unit (e.g. operator=)

#if 0
// TODO: remove
//! This block contains the old-style STL-support, and has been superseeded by
//! the generic reflection support.

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
#endif

////////////////////////////////////////////////////////////////////////////////
//! std::vector                                                               //
////////////////////////////////////////////////////////////////////////////////

//! member-types
// REFLECTABLE(auto, value_type);
// REFLECTABLE(auto, allocator_type);
// REFLECTABLE(auto, size_type);
// REFLECTABLE(auto, difference_type);
// REFLECTABLE(auto, reference);
// REFLECTABLE(auto, const_reference);
// REFLECTABLE(auto, pointer);
// REFLECTABLE(auto, const_pointer);
// REFLECTABLE(auto, iterator);
// REFLECTABLE(auto, const_iterator);
// REFLECTABLE(auto, reverse_iterator);
// REFLECTABLE(auto, const_reverse_iterator);

//! member-functions
// TODO: (constructor?)
REFLECTABLE(auto, assign);
REFLECTABLE(auto, get_allocator);
REFLECTABLE(auto, at);
REFLECTABLE(auto, front);
REFLECTABLE(auto, back);
REFLECTABLE(auto, data);
REFLECTABLE(auto, begin);
REFLECTABLE(auto, cbegin);
REFLECTABLE(auto, end);
REFLECTABLE(auto, cend);
REFLECTABLE(auto, rbegin);
REFLECTABLE(auto, crbegin);
REFLECTABLE(auto, rend);
REFLECTABLE(auto, crend);
REFLECTABLE(auto, empty);
REFLECTABLE(auto, size);
REFLECTABLE(auto, max_size);
REFLECTABLE(auto, reserve);
REFLECTABLE(auto, capacity);
REFLECTABLE(auto, shrink_to_fit);
REFLECTABLE(auto, clear);
REFLECTABLE(auto, insert);
REFLECTABLE(auto, emplace);
REFLECTABLE(auto, erase);
REFLECTABLE(auto, push_back);
REFLECTABLE(auto, emplace_back);
REFLECTABLE(auto, pop_back);
REFLECTABLE(auto, resize);
REFLECTABLE(auto, swap);

//! operators
// REFLECTABLE(auto, operator[], member_function);
REFLECTION_REFLECTABLE_NONINTRUSIVE_ASSIGNMENT_OPERATOR(auto, operator=, member_function);
// REFLECTABLE(auto, operator=,  member_function);
