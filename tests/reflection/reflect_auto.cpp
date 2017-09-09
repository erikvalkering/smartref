#include <reflection/reflect_auto.h>

namespace tests_reflect_auto {

struct foo {};

using namespace reflection;

static_assert(REFLECTION_REFLECT_AUTO(foo)    == reflect<foo>);
static_assert(REFLECTION_REFLECT_AUTO(auto_)  == reflect<auto_>);
static_assert(REFLECTION_REFLECT_AUTO(auto)   == reflect<auto_>);

} // namespace tests_reflect_auto
