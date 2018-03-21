#pragma once

#include "reflect_auto.h"
#include "reflectable_common.h"
#include "reify.h"

#include <utils/counter.h>

namespace reflection {

template<typename T, size_t counter>
struct reflected_member
{
  using type = void;
};

template<typename T, size_t counter>
using reflected_member_t = typename reflected_member<T, counter>::type;

template<typename T>
constexpr auto reflected_member_count_v = reflected_member_count<reflected_member_t, T>::value;

} // namespace reflection

// TODO: -cmaster Have a quick look whether we can simplify these macros.
// TODO: -cmaster We can replace the using type = ... with a simple boolean, such that we can inline the reflectors
#define REFLECTION_REFLECTABLE_UNIFIED(Class, member, MEMBER_TYPE_REFLECTOR, MEMBER_FUNCTION_REFLECTOR) \
  constexpr auto CONCAT(reflection, __LINE__) =                                                         \
    REFLECTION_REFLECT_AUTO(Class);                                                                     \
                                                                                                        \
  using CONCAT(CLASS, __LINE__) =                                                                       \
    decltype(reify(CONCAT(reflection, __LINE__)));                                                      \
                                                                                                        \
  template<>                                                                                            \
  struct reflection::reflected_member<                                                                  \
    CONCAT(CLASS, __LINE__),                                                                            \
    CURRENT_COUNTER(CONCAT(CLASS, __LINE__))                                                            \
  >                                                                                                     \
  {                                                                                                     \
    using type = struct                                                                                 \
    {                                                                                                   \
      MEMBER_TYPE_REFLECTOR(                                                                            \
        reflector_member_type,                                                                          \
        member);                                                                                        \
                                                                                                        \
      MEMBER_FUNCTION_REFLECTOR(                                                                        \
        reflector_member_function,                                                                      \
        member);                                                                                        \
    };                                                                                                  \
  };                                                                                                    \
                                                                                                        \
  INC_COUNTER(CONCAT(CLASS, __LINE__))                                                                  \

#define REFLECTION_REFLECTABLE2(Class, member)            \
  REFLECTION_REFLECTABLE_UNIFIED(                         \
    Class,                                                \
    member,                                               \
    REFLECTION_REFLECTABLE_ADD_MEMBER_TYPE_REFLECTOR,     \
    REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR  \
  )                                                       \

#define REFLECTION_REFLECTABLE1(member) \
  REFLECTION_REFLECTABLE2(auto, member) \

#define REFLECTION_REFLECTABLE_OPERATOR2(Class, member)   \
  REFLECTION_REFLECTABLE_UNIFIED(                         \
    Class,                                                \
    member,                                               \
    REFLECTION_REFLECTABLE_ADD_EMPTY_REFLECTOR,           \
    REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR  \
  )                                                       \

#define REFLECTION_REFLECTABLE_OPERATOR1(member)  \
  REFLECTION_REFLECTABLE_OPERATOR2(auto, member)  \

#define REFLECTION_REFLECTABLE_OPERATOR_UNARY2(Class, member)         \
  REFLECTION_REFLECTABLE_UNIFIED(                                     \
    Class,                                                            \
    member,                                                           \
    REFLECTION_REFLECTABLE_ADD_EMPTY_REFLECTOR,                       \
    REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR_NON_TEMPLATE \
  )                                                                   \

#define REFLECTION_REFLECTABLE_OPERATOR_UNARY1(member)  \
  REFLECTION_REFLECTABLE_OPERATOR_UNARY2(auto, member)  \

#define REFLECTION_REFLECTABLE_OPERATOR_ASSIGNMENT2(Class, member)            \
  REFLECTION_REFLECTABLE_UNIFIED(                                             \
    Class,                                                                    \
    member,                                                                   \
    REFLECTION_REFLECTABLE_ADD_EMPTY_REFLECTOR,                               \
    REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR_ASSIGNMENT_OPERATOR  \
  )                                                                           \

#define REFLECTION_REFLECTABLE_OPERATOR_ASSIGNMENT1(member) \
  REFLECTION_REFLECTABLE_OPERATOR_ASSIGNMENT2(auto, member) \

#define REFLECTION_REFLECTABLE_MAKE_OVERLOAD(_1, _2, NAME, ...) NAME

//! Define a set of overloads such that we can use REFLECT both with and without
//! specifying a class, by dispatching on the number of arguments.
//!
//! REFLECTION_REFLECTABLE(bar) -> REFLECTION_REFLECTABLE(auto, bar)

#define REFLECTION_REFLECTABLE(...)     \
  REFLECTION_REFLECTABLE_MAKE_OVERLOAD( \
    __VA_ARGS__,                        \
    REFLECTION_REFLECTABLE2,            \
    REFLECTION_REFLECTABLE1             \
  )(__VA_ARGS__)                        \

#define REFLECTION_REFLECTABLE_OPERATOR(...)  \
  REFLECTION_REFLECTABLE_MAKE_OVERLOAD(       \
    __VA_ARGS__,                              \
    REFLECTION_REFLECTABLE_OPERATOR2,         \
    REFLECTION_REFLECTABLE_OPERATOR1          \
  )(__VA_ARGS__)                              \

#define REFLECTION_REFLECTABLE_OPERATOR_UNARY(...)  \
  REFLECTION_REFLECTABLE_MAKE_OVERLOAD(             \
    __VA_ARGS__,                                    \
    REFLECTION_REFLECTABLE_OPERATOR_UNARY2,         \
    REFLECTION_REFLECTABLE_OPERATOR_UNARY1          \
  )(__VA_ARGS__)                                    \

#define REFLECTION_REFLECTABLE_OPERATOR_ASSIGNMENT(...) \
  REFLECTION_REFLECTABLE_MAKE_OVERLOAD(                 \
    __VA_ARGS__,                                        \
    REFLECTION_REFLECTABLE_OPERATOR_ASSIGNMENT2,        \
    REFLECTION_REFLECTABLE_OPERATOR_ASSIGNMENT1         \
  )(__VA_ARGS__)                                        \

#ifndef REFLECTION_USE_PREFIX
#define REFLECTABLE(...) REFLECTION_REFLECTABLE(__VA_ARGS__)
#define REFLECTABLE_OPERATOR(...) REFLECTION_REFLECTABLE_OPERATOR(__VA_ARGS__)
#define REFLECTABLE_OPERATOR_UNARY(...) REFLECTION_REFLECTABLE_OPERATOR_UNARY(__VA_ARGS__)
#define REFLECTABLE_OPERATOR_ASSIGNMENT(...) REFLECTION_REFLECTABLE_OPERATOR_ASSIGNMENT(__VA_ARGS__)
#endif
