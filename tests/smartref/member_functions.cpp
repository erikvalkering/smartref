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

struct EmptyClass {};

struct NonConstClass
{
  void member() {}
};

struct ConstClass
{
  void member() const {}
};

struct MixedClass
{
  void member() {}
  void member() const {}
};

////////////////////////////////
// non-const member functions //
////////////////////////////////

static_assert(!has_member<EmptyClass>(0),
              "TEST FAILED: EmptyClass seems to have a member-function!");

static_assert(!has_member<using_<EmptyClass>>(0),
              "TEST FAILED: using_<EmptyClass> seems to have a member-function!");

static_assert(has_member<NonConstClass>(0),
              "TEST FAILED: NonConstClass doesn't seem to have a member-function!");

static_assert(has_member<using_<NonConstClass>>(0),
              "TEST FAILED: using_<NonConstClass> doesn't seem to have a member-function!");

static_assert(has_member<ConstClass>(0),
              "TEST FAILED: ConstClass doesn't seem to have a member-function!");

// TODO: This reifies a non-const member-function that delegates to the const member-function
//       It should reify a const member-function
static_assert(has_member<using_<ConstClass>>(0),
              "TEST FAILED: using_<ConstClass> doesn't seem to have a member-function!");

static_assert(has_member<MixedClass>(0),
              "TEST FAILED: MixedClass doesn't seem to have a member-function!");

static_assert(has_member<using_<MixedClass>>(0),
              "TEST FAILED: using_<MixedClass> doesn't seem to have a member-function!");

////////////////////////////
// const member functions //
////////////////////////////

static_assert(!has_member<const EmptyClass>(0),
              "TEST FAILED: const EmptyClass seems to have a const member-function!");

static_assert(!has_member<const using_<EmptyClass>>(0),
              "TEST FAILED: const using_<EmptyClass> seems to have a const member-function!");

static_assert(!has_member<const NonConstClass>(0),
              "TEST FAILED: const NonConstClass seems to have a const member-function!");

static_assert(!has_member<const using_<NonConstClass>>(0),
              "TEST FAILED: const using_<NonConstClass> seem to have a const member-function!");

} // namespace test_existence
} // namespace tests
