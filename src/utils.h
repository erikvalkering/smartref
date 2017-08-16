#pragma once

#include <utility>

namespace utils {

template<typename L1, typename L2>
struct Combiner : L1, L2
{
    constexpr Combiner(L1 l1, L2 l2) : L1(std::move(l1)), L2(std::move(l2)) {}

    using L1::operator();
    using L2::operator();
};

// TODO: Replace with C++17 class template deduction
template<typename L1, typename L2>
constexpr auto make_combiner(L1 &&l1, L2 &&l2)
{
    return Combiner<std::decay_t<L1>, std::decay_t<L2>>{std::forward<L1>(l1), std::forward<L2>(l2)};
}

template<typename...>
using void_t = void;

} // namespace utils
