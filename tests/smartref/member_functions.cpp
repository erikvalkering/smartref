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

struct Empty {};

struct NonConst
{
  void foo() {}
};

struct Const
{
  void foo() const {}
};

struct Mixed
{
  void foo() {}
  void foo() const {}
};

////////////////////////////////
// non-const member functions //
////////////////////////////////

static_assert(!has_foo<Empty>(0),
              "TEST FAILED: Empty seems to have a foo() member-function!");

static_assert(!has_foo<using_<Empty>>(0),
              "TEST FAILED: using_<Empty> seems to have a foo() member-function!");

static_assert(has_foo<NonConst>(0),
              "TEST FAILED: NonConst doesn't seem to have a foo() member-function!");

static_assert(has_foo<using_<NonConst>>(0),
              "TEST FAILED: using_<NonConst> doesn't seem to have a foo() member-function!");

static_assert(has_foo<Mixed>(0),
              "TEST FAILED: Mixed doesn't seem to have a foo() member-function!");

static_assert(has_foo<using_<Mixed>>(0),
              "TEST FAILED: using_<Mixed> doesn't seem to have a foo() member-function!");

////////////////////////////
// const member functions //
////////////////////////////

static_assert(!has_foo<const Empty>(0),
              "TEST FAILED: const Empty seems to have a foo() member-function!");

static_assert(!has_foo<const using_<Empty>>(0),
              "TEST FAILED: const using_<Empty> seems to have a foo() member-function!");

static_assert(!has_foo<const NonConst>(0),
              "TEST FAILED: const NonConst seems to have a foo() const member-function!");

static_assert(!has_foo<const using_<NonConst>>(0),
              "TEST FAILED: const using_<NonConst> seem to have a foo() const member-function!");

} // namespace test_existence
} // namespace tests
