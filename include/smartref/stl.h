#pragma once

#include <reflection/reflection.h>

////////////////////////////////////////////////////////////////////////////////
//! std::vector                                                               //
////////////////////////////////////////////////////////////////////////////////

//! member-types
REFLECTABLE(value_type);
REFLECTABLE(allocator_type);
REFLECTABLE(size_type);
REFLECTABLE(difference_type);
REFLECTABLE(reference);
REFLECTABLE(const_reference);
REFLECTABLE(pointer);
REFLECTABLE(const_pointer);
REFLECTABLE(iterator);
REFLECTABLE(const_iterator);
REFLECTABLE(reverse_iterator);
REFLECTABLE(const_reverse_iterator);

//! member-functions
// TODO: (constructor?)
REFLECTABLE(assign);
REFLECTABLE(get_allocator);
REFLECTABLE(at);
REFLECTABLE(front);
REFLECTABLE(back);
REFLECTABLE(data);
REFLECTABLE(begin);
REFLECTABLE(cbegin);
REFLECTABLE(end);
REFLECTABLE(cend);
REFLECTABLE(rbegin);
REFLECTABLE(crbegin);
REFLECTABLE(rend);
REFLECTABLE(crend);
REFLECTABLE(empty);
REFLECTABLE(size);
REFLECTABLE(max_size);
REFLECTABLE(reserve);
REFLECTABLE(capacity);
REFLECTABLE(shrink_to_fit);
REFLECTABLE(clear);
REFLECTABLE(insert);
REFLECTABLE(emplace);
REFLECTABLE(erase);
REFLECTABLE(push_back);
REFLECTABLE(emplace_back);
REFLECTABLE(pop_back);
REFLECTABLE(resize);
REFLECTABLE(swap);

//! operators
REFLECTABLE_OPERATOR(operator[]);
REFLECTABLE_OPERATOR_ASSIGNMENT(operator=);
