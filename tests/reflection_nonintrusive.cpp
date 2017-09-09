#include "reflection_nonintrusive.h"

#include <type_traits>

namespace tests_reflection_nonintrusive {

struct Foo {};

static_assert(!REFLECTION_IS_AUTO(Foo));
static_assert(!REFLECTION_IS_AUTO(reflection::auto_));
static_assert( REFLECTION_IS_AUTO(auto));

} // namespace tests_reflection_nonintrusive
