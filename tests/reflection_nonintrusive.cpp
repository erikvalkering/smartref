#include <reflection/reflect.h>
#include <reflection_nonintrusive.h>

namespace tests_reflection_nonintrusive {

struct Foo {};

using namespace reflection;

static_assert(REFLECTION_SAFE_REFLECT(Foo)    == reflect<Foo>);
static_assert(REFLECTION_SAFE_REFLECT(auto_)  == reflect<auto_>);
static_assert(REFLECTION_SAFE_REFLECT(auto)   == reflect<auto_>);

} // namespace tests_reflection_nonintrusive
