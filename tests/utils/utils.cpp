#include <utils/utils.h>

using namespace utils;

static_assert(std::is_same<like_t<const int &&, float>, const float &&>::value);

template<typename>
struct Predicate : std::false_type {};

template<>
struct Predicate<int> : std::true_type {};

static_assert(std::is_same<find<Predicate, float, bool, int>, int>::value);
