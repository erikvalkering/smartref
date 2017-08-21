#include "reflection.h"

struct Foo {};
static_assert(std::is_same<void, reflection::reflected_member_t<Foo, 0>>::value);
static_assert(reflection::reflected_member_count_v<Foo> == 0);

struct Bar
{
    void bar();
};
REFLECT(Bar, bar);
static_assert(!std::is_same<void, reflection::reflected_member_t<Bar, 0>>::value);
static_assert( std::is_same<void, reflection::reflected_member_t<Bar, 1>>::value);
static_assert(reflection::reflected_member_count_v<Bar> == 1);

struct Baz
{
    void baz();
    void baz2();
};
REFLECT(Baz, baz);
REFLECT(Baz, baz2);
static_assert(!std::is_same<void, reflection::reflected_member_t<Baz, 0>>::value);
static_assert(!std::is_same<void, reflection::reflected_member_t<Baz, 1>>::value);
static_assert( std::is_same<void, reflection::reflected_member_t<Baz, 2>>::value);
static_assert(reflection::reflected_member_count_v<Baz> == 2);

struct Bat
{
    void REFLECT(bat)();
    void REFLECT(bat2)();
};

static_assert(!std::is_same<void, reflection::reflected_class_member_t<Bat, 0>>::value);
static_assert(!std::is_same<void, reflection::reflected_class_member_t<Bat, 1>>::value);
static_assert( std::is_same<void, reflection::reflected_class_member_t<Bat, 2>>::value);
static_assert(reflection::reflected_class_member_count_v<Bat> == 2);

namespace member_types {

struct Foo
{
    void member_function() {}
    using member_type = void;
};

} // namespace member_types

REFLECT(member_types::Foo, member_function);
REFLECT(member_types::Foo, member_type);

using reflected_member_function = reflection::reflected_member_t<member_types::Foo, 0>;
using reflected_member_type = reflection::reflected_member_t<member_types::Foo, 1>;

static_assert(reflection::reflected_kind_v<reflected_member_function> == reflection::reflected_kind::member_function);
static_assert(reflection::reflected_kind_v<reflected_member_type> == reflection::reflected_kind::member_type);
