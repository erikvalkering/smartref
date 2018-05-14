#include <smartref/smartref.h>

REFLECTABLE(foo);

namespace tests {

using smartref::using_;

//! These tests check for existence of the foo member-function.
namespace test_existence {

template<typename T>
constexpr auto has_foo(int) -> decltype(std::declval<T>().foo(), bool{}) {return true;}
template<typename T>
constexpr auto has_foo(...) {return false;}

struct Foo
{
  void foo() {}
};

struct Bar {};

////////////////////////////////
// non-const member functions //
////////////////////////////////

static_assert(has_foo<Foo>(0),
              "TEST FAILED: Foo doesn't seem to have a foo() member-function!");

static_assert(has_foo<using_<Foo>>(0),
              "TEST FAILED: using_<Foo> doesn't seem to have a foo() member-function!");

static_assert(!has_foo<Bar>(0),
              "TEST FAILED: Bar seems to have a foo() member-function!");

static_assert(!has_foo<using_<Bar>>(0),
              "TEST FAILED: using_<Bar> seems to have a foo() member-function!");

////////////////////////////
// const member functions //
////////////////////////////

static_assert(!has_foo<const Foo>(0),
              "TEST FAILED: const Foo seems to have a foo() const member-function!");

static_assert(!has_foo<const using_<Foo>>(0),
              "TEST FAILED: const using_<Foo> seem to have a foo() const member-function!");

static_assert(!has_foo<const Bar>(0),
              "TEST FAILED: const Bar seems to have a foo() const member-function!");

static_assert(!has_foo<const using_<Bar>>(0),
              "TEST FAILED: const using_<Bar> seems to have a foo() const member-function!");

} // namespace test_existence
} // namespace tests
