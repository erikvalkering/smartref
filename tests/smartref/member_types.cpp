#include <smartref.h>

#include <type_traits>
#include <vector>

using smartref::using_;

static_assert(
    std::is_same<
        using_<std::vector<int>>::value_type,
               std::vector<int> ::value_type
    >::value);
