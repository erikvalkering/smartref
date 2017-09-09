#include <reflection/reflect.h>

namespace tests_reflect {

using namespace reflection;

static_assert(  reflect<int> == reflect<int>);
static_assert(!(reflect<int> != reflect<int>));

static_assert(!(reflect<int> == reflect<float>));
static_assert(  reflect<int> != reflect<float>);

} // namespace tests_reflect
