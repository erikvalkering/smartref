#pragma once

#include "cpp17.h"

#include <utility>
#include <type_traits>

#define CONCAT2(x, y) x ## y
#define CONCAT(x, y) CONCAT2(x, y)

#define SFINAEABLE_RETURN(...)  \
    -> decltype(__VA_ARGS__)    \
  {                             \
    return __VA_ARGS__;         \
  }                             \

#define RETURN(...)     \
  {                     \
    return __VA_ARGS__; \
  }                     \

#define CONSTRAINED_SFINAEABLE_RETURN(constraint, ...)  \
    -> std::enable_if_t<                                \
      constraint,                                       \
      decltype(__VA_ARGS__)                             \
    >                                                   \
  {                                                     \
    return __VA_ARGS__;                                 \
  }                                                     \

namespace utils {

template<typename...>
struct type_list {};

template<typename L1, typename L2>
struct Combiner : L1, L2
{
  constexpr Combiner(L1 l1, L2 l2) : L1(std::move(l1)), L2(std::move(l2)) {}

  using L1::operator();
  using L2::operator();
};

template<typename L1, typename L2>
constexpr auto make_combiner(L1 &&l1, L2 &&l2)
{
  return Combiner<std::decay_t<L1>, std::decay_t<L2>>{std::forward<L1>(l1), std::forward<L2>(l2)};
}

// template<typename... Callables>
// struct overload : Callables...
// {
//   using Callables::operator()...;
// };

// template<typename... Callables>
// overload(Callables...) -> overload<Callables...>;

template<class... Bases>
struct Compose : Bases...
{
  Compose() = default;
  Compose(const Compose &) = default;
  Compose(Compose &&) = default;
  Compose &operator=(const Compose &) = default;
  Compose &operator=(Compose &&) = default;

  using Bases::operator=...;
};

template<typename MetaFunction, typename Arg>
using apply = typename MetaFunction::template type<Arg>;

template<template<typename> class MetaFunction>
struct metafunction
{
  template<typename Arg>
  using type = MetaFunction<Arg>;
};

template<typename... BaseConstructors>
struct ClassConstructor : apply<BaseConstructors, ClassConstructor<BaseConstructors...>>...
{
  ClassConstructor() = default;
  ClassConstructor(const ClassConstructor &) = default;
  ClassConstructor(ClassConstructor &&) = default;
  ClassConstructor &operator=(const ClassConstructor &) = default;
  ClassConstructor &operator=(ClassConstructor &&) = default;

  using apply<BaseConstructors, ClassConstructor<BaseConstructors...>>::operator=...;
};

template<class Class, typename... T>
struct DelayedImpl
{
  using type = Class;
};

template<class Class, typename... T>
using Delayed = typename DelayedImpl<Class, T...>::type;

template<typename... T, typename Obj>
decltype(auto) delayed(Obj &&obj, ...)
{
  return std::forward<Obj>(obj);
}

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

template<typename... Ts>
constexpr auto always_true = true;

template<typename... Ts>
constexpr auto always_false = false;

template<template<typename...> class T, typename... Us>
constexpr auto pack_size(T<Us...>)
{
  return sizeof...(Us);
}

template<typename T, typename... Ts>
constexpr auto none_of = (!std::is_same<T, Ts>::value && ...);

template<typename T, typename... Ts>
constexpr auto any_of = !none_of<T, Ts...>;

template<template<typename> class, typename...>
struct FindImpl;

template<template<typename> class Predicate>
struct FindImpl<Predicate>
{
  using type = void;
};

template<template<typename> class Predicate, typename T, typename... Ts>
struct FindImpl<Predicate, T, Ts...>
{
  using type = std::conditional_t<
    Predicate<T>::value,
    T,
    typename FindImpl<Predicate, Ts...>::type
  >;
};

template<template<typename> class Predicate, typename... Ts>
using find = typename FindImpl<Predicate, Ts...>::type;

// TODO: Move to cpp20.h
//! C++20 type trait (from cppreference.com)
template<typename T>
struct remove_cvref
{
  using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template<typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

template<typename T, typename U>
struct like
{
  using stripped_U = remove_cvref_t<U>;

  using qualified_U = std::conditional_t<
    std::is_const<std::remove_reference_t<T>>::value,
    const stripped_U,
    stripped_U
  >;

  using type = std::conditional_t<
    std::is_lvalue_reference<T>::value,
    qualified_U &,
    qualified_U &&
  >;
};

template<typename T, typename U>
using like_t = typename like<T, U>::type;

} // namespace utils

template<typename T>
using name = typename T::asdf;
