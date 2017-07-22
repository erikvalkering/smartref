#pragma once

//! Compile-time counter. Credits go to the folks at CopperSpice (see CppCon2015 talk).

#include <cstddef>
#include <utility>

namespace counter {

// TODO: multiple inheritance to detect size

template<size_t count, typename T = void>
struct Counter : Counter<count - 1, T>
{
    static constexpr auto value = count;
};

template<typename T>
struct Counter<0, T>
{
    static constexpr auto value = 0;
};

template<typename T>
Counter<0, T> __counter(Counter<0, T>);

template<typename L1, typename L2>
struct Combiner : L1, L2
{
    constexpr Combiner(L1 l1, L2 l2) : L1(std::move(l1)), L2(std::move(l2)) {}

    using L1::operator();
    using L2::operator();
};

template<typename L1, typename L2>
constexpr auto make_combiner(L1 &&l1, L2 &&l2)
{
    return Combiner<std::decay_t<L1>, std::decay_t<L2>>{std::forward<L1>(l1), std::forward<L2>(l2)};
}

template<typename F>
constexpr auto current_class_counter(F f)
{
    auto fallback = [](counter::Counter<0> counter)
    {
        return counter;
    };

    auto combined = make_combiner(f, fallback);

    return decltype(combined(counter::Counter<255>{}))::value;
}

} // namespace counter

#define TOKENPASTE2(x, y) x ## y
#define TOKENPASTE(x, y) TOKENPASTE2(x, y)

#define CURRENT_COUNTER_IMPL2(T, FUNCTION)  \
    decltype(                               \
        counter::current_counter_helper<T>( \
            [](auto counter)                \
            {                               \
                return FUNCTION(counter);   \
            })                              \
        )::value                            \

#define CURRENT_COUNTER_IMPL(T, FUNCTION)                   \
    decltype(FUNCTION(counter::Counter<255, T>{}))::value   \

#define CURRENT_COUNTER(T)              \
    CURRENT_COUNTER_IMPL(T, __counter)  \

#define INC_COUNTER(T)                                                                                      \
    namespace counter {                                                                                     \
    constexpr auto TOKENPASTE(value_, __LINE__) = CURRENT_COUNTER(T);                                       \
    Counter<TOKENPASTE(value_, __LINE__) + 1, T> __counter(Counter<TOKENPASTE(value_, __LINE__) + 1, T>);   \
    } /* namespace counter */                                                                               \

#define CURRENT_CLASS_COUNTER()                                 \
    counter::current_class_counter(                             \
        [](auto counter) -> decltype(__class_counter(counter))  \
        {                                                       \
            return __class_counter(counter);                    \
        })                                                      \

#define INC_CLASS_COUNTER()                                                                     \
    static constexpr auto TOKENPASTE(value_, __LINE__) = CURRENT_CLASS_COUNTER();               \
    static constexpr auto __class_counter(counter::Counter<TOKENPASTE(value_, __LINE__) + 1>)   \
        -> counter::Counter<TOKENPASTE(value_, __LINE__) + 1>                                   \
    {                                                                                           \
        return {};                                                                              \
    }                                                                                           \
