#pragma once

//! Compile-time counter. Credits go to the folks at CopperSpice (see CppCon2015 talk).

namespace counter {

template<typename T, size_t count>
struct Counter : Counter<T, count - 1>
{
    static constexpr auto value = count;
};

template<typename T>
struct Counter<T, 0>
{
    static constexpr auto value = 0;
};

template<typename T>
Counter<T, 0> counter(Counter<T, 0>);

#define TOKENPASTE2(x, y) x ## y
#define TOKENPASTE(x, y) TOKENPASTE2(x, y)

#define CURRENT_COUNTER(T)                    \
    decltype(counter(Counter<T, 255>{}))::value \

#define INC_COUNTER(T)                                                                                 \
    constexpr auto TOKENPASTE(value_, __LINE__) = CURRENT_COUNTER(T);                                  \
    Counter<T, TOKENPASTE(value_, __LINE__) + 1> counter(Counter<T, TOKENPASTE(value_, __LINE__) + 1>) \

static_assert(CURRENT_COUNTER(void) == 0);
static_assert(CURRENT_COUNTER(void) == 0);
INC_COUNTER(void);
static_assert(CURRENT_COUNTER(void) == 1);
static_assert(CURRENT_COUNTER(void) == 1);
INC_COUNTER(void);
static_assert(CURRENT_COUNTER(void) == 2);
static_assert(CURRENT_COUNTER(void) == 2);

} // namespace counter
