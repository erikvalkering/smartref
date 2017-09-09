#include "reflection_nonintrusive.h"

#include <type_traits>

namespace tests_reflection_nonintrusive {

struct Foo {};

static_assert(!REFLECTION_IS_AUTO_KEYWORD(Foo));
static_assert(!REFLECTION_IS_AUTO_KEYWORD(reflection::auto_));
static_assert( REFLECTION_IS_AUTO_KEYWORD(auto));

} // namespace tests_reflection_nonintrusive
