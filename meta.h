#pragma once

namespace meta {

template<typename T>
struct MetaType
{
  constexpr auto operator==(MetaType) const { return true; }
  constexpr auto operator!=(MetaType) const { return false; }

  template<typename U>
  constexpr auto operator==(U) const { return false; }
  template<typename U>
  constexpr auto operator!=(U) const { return true; }
};

template<typename T>
constexpr auto Type = MetaType<T>{};

template<typename T>
constexpr auto type(T)
{
  return Type<T>;
}

} // namespace meta

//! Tests for comparison of types using meta::Type<>
static_assert(meta::Type<int>   == meta::Type<int>);
static_assert(meta::Type<int>   != meta::Type<float>);
static_assert(meta::Type<float> == meta::Type<float>);

//! Tests for comparison of values' types using meta::type()
static_assert(meta::type(int{}) == meta::type(int{}));
static_assert(meta::type(int{}) != meta::type(float{}));
static_assert(meta::type(float{}) == meta::type(float{}));

//! Tests for comparison of lambda's types using meta::type()
constexpr auto lambda1 = []{};
constexpr auto lambda2 = []{};
static_assert(meta::type(lambda1) == meta::type(lambda1));
static_assert(meta::type(lambda1) != meta::type(lambda2));
static_assert(meta::type(lambda2) == meta::type(lambda2));

//! Tests for comparison of lambda expressions' types using meta::type()
static_assert(meta::type([]{})            != meta::type([]{}));
static_assert(meta::type([]{return 0;}()) == meta::type([]{return 0;}()));
