#pragma once

//! Compile-time counter. Credits go to the folks at CopperSpice (see CppCon2015 talk).

#include "utils.h"

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

template<typename F>
constexpr auto current_class_counter(F f)
{
    auto fallback = [](counter::Counter<0> counter)
    {
        return counter;
    };

    auto combined = utils::make_combiner(f, fallback);

    return decltype(combined(counter::Counter<255>{}))::value;
}

} // namespace counter

#define CONCAT2(x, y) x ## y
#define CONCAT(x, y) CONCAT2(x, y)

#define CURRENT_COUNTER_IMPL(T, FUNCTION)                   \
    decltype(FUNCTION(counter::Counter<255, T>{}))::value   \

#define CURRENT_COUNTER(T)              \
    CURRENT_COUNTER_IMPL(T, __counter)  \

#define INC_COUNTER(T)                                                                                      \
    namespace counter {                                                                                     \
    constexpr auto CONCAT(value_, __LINE__) = CURRENT_COUNTER(T);                                       \
    Counter<CONCAT(value_, __LINE__) + 1, T> __counter(Counter<CONCAT(value_, __LINE__) + 1, T>);   \
    } /* namespace counter */                                                                               \

#define CURRENT_CLASS_COUNTER()                                 \
    counter::current_class_counter(                             \
        [](auto counter) -> decltype(__class_counter(counter))  \
        {                                                       \
            return __class_counter(counter);                    \
        })                                                      \

#define INC_CLASS_COUNTER()                                                                     \
    static constexpr auto CONCAT(value_, __LINE__) = CURRENT_CLASS_COUNTER();               \
    static constexpr auto __class_counter(counter::Counter<CONCAT(value_, __LINE__) + 1>)   \
        -> counter::Counter<CONCAT(value_, __LINE__) + 1>                                   \
    {                                                                                           \
        return {};                                                                              \
    }                                                                                           \

// EXTRA NEWLINE FOR GCC WHICH HAS PROBLEMS WITH MULTILINE MACROS
