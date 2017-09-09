#pragma once

#include "reflection/reflect.h"
#include "reflection/reflect_members.h"
#include "reflection/reify.h"

//! Define a set of overloads such that we can use REFLECT both inside a class,
//! as well as outside, by dispatching on the number of arguments.
//!
//! REFLECTION_REFLECTABLE(bar) -> REFLECTION_REFLECTABLE_INTRUSIVE(bar)
//! REFLECTION_REFLECTABLE(Foo, bar) -> REFLECTION_REFLECTABLE_NONINTRUSIVE(Foo, bar)
#define REFLECTION_MAKE_OVERLOAD(_1, _2, NAME, ...) NAME
#define REFLECTION_REFLECTABLE(...)           \
    REFLECTION_MAKE_OVERLOAD(                 \
        __VA_ARGS__,                          \
        REFLECTION_REFLECTABLE_NONINTRUSIVE,  \
        REFLECTION_REFLECTABLE_INTRUSIVE      \
    )(__VA_ARGS__)                            \

#ifndef REFLECTION_USE_PREFIX
#define REFLECTABLE(...) REFLECTION_REFLECTABLE(__VA_ARGS__)
#endif
