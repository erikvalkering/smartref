#pragma once

#include "cpp17.h"

#include <utility>
#include <type_traits>

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

// TODO: Replace with C++17 class template deduction
template<typename L1, typename L2>
constexpr auto make_combiner(L1 &&l1, L2 &&l2)
{
  return Combiner<std::decay_t<L1>, std::decay_t<L2>>{std::forward<L1>(l1), std::forward<L2>(l2)};
}

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

template<typename... Ts>
constexpr auto always_true = true;

template<template<typename...> class T, typename... Us>
constexpr auto pack_size(T<Us...>)
{
  return sizeof...(Us);
}

} // namespace utils

#define CONCAT2(x, y) x ## y
#define CONCAT(x, y) CONCAT2(x, y)
