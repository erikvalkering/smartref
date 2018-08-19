#pragma once

#include "reflect_auto.h"
#include "reflectors.h"
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

template<
  template<typename, size_t> class reflected_member_slot_t,
  typename T,
  size_t count = 0,
  typename = reflected_member_slot_t<T, count>
>
struct reflected_member_count
{
  static constexpr auto value = reflected_member_count<reflected_member_slot_t, T, count + 1>::value;
};

template<
  template<typename, size_t> class reflected_member_slot_t,
  typename T,
  size_t count
>
struct reflected_member_count<reflected_member_slot_t, T, count, void>
{
  static constexpr auto value = count;
};

template<typename T>
constexpr auto reflected_member_count_v = reflected_member_count<reflected_member_t, T>::value;

} // namespace reflection

// TODO: -cmaster Have a quick look whether we can simplify these macros.
// TODO: -cmaster We can replace the using type = ... with a simple boolean, such that we can inline the reflectors
// TODO: Rename 'member' to more general 'name' (for free function 'member' is nonsense)
#define REFLECTION_REFLECTABLE_UNIFIED(Class, member, PREAMBLE,               \
  MEMBER_TYPE_REFLECTOR, MEMBER_FUNCTION_REFLECTOR, FREE_FUNCTION_REFLECTOR   \
)                                                                             \
  PREAMBLE(member);                                                           \
                                                                              \
  constexpr auto CONCAT(reflection, __LINE__) =                               \
    REFLECTION_REFLECT_AUTO(Class);                                           \
                                                                              \
  using CONCAT(CLASS, __LINE__) =                                             \
    decltype(reify(CONCAT(reflection, __LINE__)));                            \
                                                                              \
  template<>                                                                  \
  struct reflection::reflected_member<                                        \
    CONCAT(CLASS, __LINE__),                                                  \
    CURRENT_COUNTER(CONCAT(CLASS, __LINE__))                                  \
  >                                                                           \
  {                                                                           \
    using type = struct                                                       \
    {                                                                         \
      MEMBER_TYPE_REFLECTOR(                                                  \
        reflector_member_type,                                                \
        member);                                                              \
                                                                              \
      MEMBER_FUNCTION_REFLECTOR(                                              \
        reflector_member_function,                                            \
        member);                                                              \
                                                                              \
      FREE_FUNCTION_REFLECTOR(                                                \
        reflector_free_function,                                              \
        member);                                                              \
    };                                                                        \
  };                                                                          \
                                                                              \
  INC_COUNTER(CONCAT(CLASS, __LINE__))                                        \

#define REFLECTION_REFLECTABLE2(Class, member)            \
  REFLECTION_REFLECTABLE_UNIFIED(                         \
    Class,                                                \
    member,                                               \
    REFLECTION_REFLECTABLE_ADD_FREE_FUNCTION_PREAMBLE,    \
    REFLECTION_REFLECTABLE_ADD_MEMBER_TYPE_REFLECTOR,     \
    REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR, \
    REFLECTION_REFLECTABLE_ADD_FREE_FUNCTION_REFLECTOR    \
  )                                                       \

#define REFLECTION_REFLECTABLE1(member) \
  REFLECTION_REFLECTABLE2(auto, member) \

#define REFLECTION_REFLECTABLE_OPERATOR2(Class, member)   \
  REFLECTION_REFLECTABLE_UNIFIED(                         \
    Class,                                                \
    member,                                               \
    REFLECTION_REFLECTABLE_ADD_EMPTY_PREAMBLE,            \
    REFLECTION_REFLECTABLE_ADD_EMPTY_REFLECTOR,           \
    REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR, \
    REFLECTION_REFLECTABLE_ADD_EMPTY_REFLECTOR            \
  )                                                       \

#define REFLECTION_REFLECTABLE_OPERATOR1(member)  \
  REFLECTION_REFLECTABLE_OPERATOR2(auto, member)  \

#define REFLECTION_REFLECTABLE_OPERATOR_UNARY2(Class, member)           \
  REFLECTION_REFLECTABLE_UNIFIED(                                       \
    Class,                                                              \
    member,                                                             \
    REFLECTION_REFLECTABLE_ADD_EMPTY_PREAMBLE,                          \
    REFLECTION_REFLECTABLE_ADD_EMPTY_REFLECTOR,                         \
    REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_NON_TEMPLATE_REFLECTOR,  \
    REFLECTION_REFLECTABLE_ADD_EMPTY_REFLECTOR                          \
  )                                                                     \

#define REFLECTION_REFLECTABLE_OPERATOR_UNARY1(member)  \
  REFLECTION_REFLECTABLE_OPERATOR_UNARY2(auto, member)  \

#define REFLECTION_REFLECTABLE_OPERATOR_INFIX2(Class, member)             \
  REFLECTION_REFLECTABLE_UNIFIED(                                         \
    Class,                                                                \
    member,                                                               \
    REFLECTION_REFLECTABLE_ADD_EMPTY_PREAMBLE,                            \
    REFLECTION_REFLECTABLE_ADD_EMPTY_REFLECTOR,                           \
    REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_OPERATOR_INFIX_REFLECTOR,  \
    REFLECTION_REFLECTABLE_ADD_EMPTY_REFLECTOR                            \
  )                                                                       \

#define REFLECTION_REFLECTABLE_OPERATOR_INFIX1(member) \
  REFLECTION_REFLECTABLE_OPERATOR_INFIX2(auto, member) \

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

#define REFLECTION_REFLECTABLE_OPERATOR_INFIX(...) \
  REFLECTION_REFLECTABLE_MAKE_OVERLOAD(                 \
    __VA_ARGS__,                                        \
    REFLECTION_REFLECTABLE_OPERATOR_INFIX2,        \
    REFLECTION_REFLECTABLE_OPERATOR_INFIX1         \
  )(__VA_ARGS__)                                        \

#ifndef REFLECTION_USE_PREFIX
#define REFLECTABLE(...) REFLECTION_REFLECTABLE(__VA_ARGS__)
#define REFLECTABLE_OPERATOR(...) REFLECTION_REFLECTABLE_OPERATOR(__VA_ARGS__)
#define REFLECTABLE_OPERATOR_UNARY(...) REFLECTION_REFLECTABLE_OPERATOR_UNARY(__VA_ARGS__)
#define REFLECTABLE_OPERATOR_INFIX(...) REFLECTION_REFLECTABLE_OPERATOR_INFIX(__VA_ARGS__)
#endif
