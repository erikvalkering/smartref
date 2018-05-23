#include <utils/utils.h>

using namespace utils;

static_assert(std::is_same<like_t<const int &&, float>, const float &&>::value);
