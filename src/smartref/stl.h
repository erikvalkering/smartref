#pragma once

#include <reflection/reflection.h>

////////////////////////////////////////////////////////////////////////////////
//! std::vector                                                               //
////////////////////////////////////////////////////////////////////////////////

//! member-types
REFLECTABLE(auto, value_type);
REFLECTABLE(auto, allocator_type);
REFLECTABLE(auto, size_type);
REFLECTABLE(auto, difference_type);
REFLECTABLE(auto, reference);
REFLECTABLE(auto, const_reference);
REFLECTABLE(auto, pointer);
REFLECTABLE(auto, const_pointer);
REFLECTABLE(auto, iterator);
REFLECTABLE(auto, const_iterator);
REFLECTABLE(auto, reverse_iterator);
REFLECTABLE(auto, const_reverse_iterator);

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
REFLECTABLE(auto, operator[], member_function);
REFLECTION_REFLECTABLE_NONINTRUSIVE_ASSIGNMENT_OPERATOR(auto, operator=, member_function);
