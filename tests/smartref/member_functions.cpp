#include <smartref/smartref.h>

REFLECTABLE(member);

namespace tests {

using smartref::using_;

//! These tests check for existence of the member() member-function.
namespace test_existence {

template<typename T>
constexpr auto has_member(int) -> decltype(std::declval<T>().member(), bool{}) {return true;}
template<typename T>
constexpr auto has_member(...) {return false;}

struct Empty {};

struct NonConst
{
  void member() {}
};

struct Const
{
  void member() const {}
};

struct Mixed
{
  void member() {}
  void member() const {}
};

////////////////////////////////
// non-const member functions //
////////////////////////////////

static_assert(!has_member<Empty>(0),
              "TEST FAILED: Empty seems to have a member-function!");

static_assert(!has_member<using_<Empty>>(0),
              "TEST FAILED: using_<Empty> seems to have a member-function!");

static_assert(has_member<NonConst>(0),
              "TEST FAILED: NonConst doesn't seem to have a member-function!");

static_assert(has_member<using_<NonConst>>(0),
              "TEST FAILED: using_<NonConst> doesn't seem to have a member-function!");

static_assert(has_member<Const>(0),
              "TEST FAILED: Const doesn't seem to have a member-function!");

// TODO: This should currently fail
static_assert(has_member<using_<Const>>(0),
              "TEST FAILED: using_<Const> doesn't seem to have a member-function!");

static_assert(has_member<Mixed>(0),
              "TEST FAILED: Mixed doesn't seem to have a member-function!");

static_assert(has_member<using_<Mixed>>(0),
              "TEST FAILED: using_<Mixed> doesn't seem to have a member-function!");

////////////////////////////
// const member functions //
////////////////////////////

static_assert(!has_member<const Empty>(0),
              "TEST FAILED: const Empty seems to have a const member-function!");

static_assert(!has_member<const using_<Empty>>(0),
              "TEST FAILED: const using_<Empty> seems to have a const member-function!");

static_assert(!has_member<const NonConst>(0),
              "TEST FAILED: const NonConst seems to have a const member-function!");

static_assert(!has_member<const using_<NonConst>>(0),
              "TEST FAILED: const using_<NonConst> seem to have a const member-function!");

} // namespace test_existence
} // namespace tests
