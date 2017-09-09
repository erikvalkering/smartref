#include "reflection.h"

#include "utils.h"

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

struct Bat
{
    void REFLECTABLE(bat)();
    void REFLECTABLE(bat2)();
};

static_assert(!std::is_same<void, reflection::reflected_member_intrusive_t<Bat, 0>>::value);
static_assert(!std::is_same<void, reflection::reflected_member_intrusive_t<Bat, 1>>::value);
static_assert( std::is_same<void, reflection::reflected_member_intrusive_t<Bat, 2>>::value);
static_assert(reflection::reflected_member_intrusive_count_v<Bat> == 2);

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

// TODO: Implementation detail. See if we can move this out of the macro
// static_assert(!utils::is_detected_v<reflected_bar::template detect_is_member_type, void>);
// static_assert( utils::is_detected_v<reflected_baz::template detect_is_member_type, void>);

// TODO: member_types cannot be detected any longer, because the underlying machinery now depends on
//       the class being passed, which is unavailable here. Of course we could pass 'Foo' as the class,
//       but the Reflection should capture this by itself.
//       Better would be not to rely on this low-level construct, which is never used anyway, and instead
//       test the high-level reflection facilities.
// static_assert(reflection::reflected_kind_v<reflected_bar> == reflection::reflected_kind::member_function);
// static_assert(reflection::reflected_kind_v<reflected_baz> != reflection::reflected_kind::unknown);
// static_assert(reflection::reflected_kind_v<reflected_baz> == reflection::reflected_kind::member_function);
// static_assert(reflection::reflected_kind_v<reflected_baz> == reflection::reflected_kind::member_type);
