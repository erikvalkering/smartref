#include <smartref/smartref.h>

#include <type_traits>

REFLECTABLE(member);

namespace tests {

using smartref::using_;

//! These tests check for existence of the member() member-function.
namespace test_existence {

template<typename T>
constexpr auto has_member(int) -> decltype(std::declval<T &>().member(), bool{}) {return true;}
template<typename T>
constexpr auto has_member(...) {return false;}

template<typename Class>
constexpr auto is_const_member_function(void (Class::*)() const) { return true;  }
constexpr auto is_const_member_function(...)                     { return false; }

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

static_assert(!has_member<using_<EmptyClass>>(0),
              "TEST FAILED: using_<EmptyClass> seems to have a member-function!");

static_assert(has_member<using_<NonConstMemberClass>>(0),
              "TEST FAILED: using_<NonConstMemberClass> doesn't seem to have a member-function!");

static_assert(has_member<using_<ConstMemberClass>>(0),
              "TEST FAILED: using_<ConstMemberClass> doesn't seem to have a member-function!");

static_assert(is_const_member_function(&using_<ConstMemberClass>::member<>),
              "TEST FAILED: using_<ConstMemberClass> doesn't seem to have a const member-function!");

static_assert(has_member<using_<MixedMemberClass>>(0),
              "TEST FAILED: using_<MixedMemberClass> doesn't seem to have a member-function!");

////////////////////////////
// const member functions //
////////////////////////////

static_assert(!has_member<const using_<EmptyClass>>(0),
              "TEST FAILED: const using_<EmptyClass> seems to have a const member-function!");

static_assert(!has_member<const using_<NonConstMemberClass>>(0),
              "TEST FAILED: const using_<NonConstMemberClass> seem to have a const member-function!");

static_assert(has_member<const using_<ConstMemberClass>>(0),
              "TEST FAILED: const using_<ConstMemberClass> doesn't seem to have a const member-function!");

static_assert(has_member<const using_<MixedMemberClass>>(0),
              "TEST FAILED: const using_<MixedMemberClass> doesn't seem to have a const member-function!");

} // namespace test_existence
} // namespace tests
