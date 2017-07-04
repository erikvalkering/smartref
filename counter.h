#pragma once

//! Compile-time counter. Credits go to the folks at CopperSpice (see CppCon2015 talk).

template<size_t count>
struct Counter : Counter<count - 1>
{
    static constexpr auto value = count;
};

template<>
struct Counter<0>
{
    static constexpr auto value = 0;
};

Counter<0> counter(Counter<0>);

#define TOKENPASTE2(x, y) x ## y
#define TOKENPASTE(x, y) TOKENPASTE2(x, y)

#define CURRENT_COUNTER()                    \
    decltype(counter(Counter<255>{}))::value \

#define INC_COUNTER()                                                              \
    constexpr auto TOKENPASTE(value_, __LINE__) = CURRENT_COUNTER();                      \
    Counter<TOKENPASTE(value_, __LINE__) + 1> counter(Counter<TOKENPASTE(value_, __LINE__) + 1>) \

static_assert(CURRENT_COUNTER() == 0);
static_assert(CURRENT_COUNTER() == 0);
INC_COUNTER();
static_assert(CURRENT_COUNTER() == 1);
static_assert(CURRENT_COUNTER() == 1);
INC_COUNTER();
static_assert(CURRENT_COUNTER() == 2);
static_assert(CURRENT_COUNTER() == 2);
