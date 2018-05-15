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

struct NonConstMemberClass
{
  void member() {}
};

struct ConstMemberClass
{
  void member() const {}
};

struct MixedMemberClass
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

static_assert(has_member<NonConstMemberClass>(0),
              "TEST FAILED: NonConstMemberClass doesn't seem to have a member-function!");

static_assert(has_member<using_<NonConstMemberClass>>(0),
              "TEST FAILED: using_<NonConstMemberClass> doesn't seem to have a member-function!");

static_assert(has_member<ConstMemberClass>(0),
              "TEST FAILED: ConstMemberClass doesn't seem to have a member-function!");

// TODO: This reifies a non-const member-function that delegates to the const member-function
//       It should reify a const member-function
static_assert(has_member<using_<ConstMemberClass>>(0),
              "TEST FAILED: using_<ConstMemberClass> doesn't seem to have a member-function!");

static_assert(has_member<MixedMemberClass>(0),
              "TEST FAILED: MixedMemberClass doesn't seem to have a member-function!");

static_assert(has_member<using_<MixedMemberClass>>(0),
              "TEST FAILED: using_<MixedMemberClass> doesn't seem to have a member-function!");

////////////////////////////
// const member functions //
////////////////////////////

static_assert(!has_member<const EmptyClass>(0),
              "TEST FAILED: const EmptyClass seems to have a const member-function!");

static_assert(!has_member<const using_<EmptyClass>>(0),
              "TEST FAILED: const using_<EmptyClass> seems to have a const member-function!");

static_assert(!has_member<const NonConstMemberClass>(0),
              "TEST FAILED: const NonConstMemberClass seems to have a const member-function!");

static_assert(!has_member<const using_<NonConstMemberClass>>(0),
              "TEST FAILED: const using_<NonConstMemberClass> seem to have a const member-function!");

} // namespace test_existence
} // namespace tests
