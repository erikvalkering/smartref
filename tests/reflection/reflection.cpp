#include <reflection/reflection.h>
#include <utils/utils.h>

struct Foo {};
static_assert(std::is_same<void, reflection::reflected_member_t<Foo, 0>>::value);
static_assert(reflection::reflected_member_count_v<Foo> == 0);

struct Bar
{
  void bar();
};
REFLECTABLE(Bar, bar);
static_assert(!std::is_same<void, reflection::reflected_member_t<Bar, 0>>::value);
static_assert( std::is_same<void, reflection::reflected_member_t<Bar, 1>>::value);
static_assert(reflection::reflected_member_count_v<Bar> == 1);

struct Baz
{
  void baz();
  void baz2();
};
REFLECTABLE(Baz, baz);
REFLECTABLE(Baz, baz2);
static_assert(!std::is_same<void, reflection::reflected_member_t<Baz, 0>>::value);
static_assert(!std::is_same<void, reflection::reflected_member_t<Baz, 1>>::value);
static_assert( std::is_same<void, reflection::reflected_member_t<Baz, 2>>::value);
static_assert(reflection::reflected_member_count_v<Baz> == 2);

namespace member_types {

struct Foo
{
  void bar() {}
  using baz = void;
};

} // namespace member_types

REFLECTABLE(member_types::Foo, bar);
REFLECTABLE(member_types::Foo, baz);

using reflected_bar = reflection::reflected_member_t<member_types::Foo, 0>;
using reflected_baz = reflection::reflected_member_t<member_types::Foo, 1>;
