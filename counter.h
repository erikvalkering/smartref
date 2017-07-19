#pragma once

//! Compile-time counter. Credits go to the folks at CopperSpice (see CppCon2015 talk).

namespace counter {

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

template<typename T, typename F>
constexpr auto current_value2(F f)
{
    return decltype(f(counter::Counter<255>{}))::value;
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

static_assert(CURRENT_COUNTER(void) == 0);
static_assert(CURRENT_COUNTER(void) == 0);
INC_COUNTER(void);
static_assert(CURRENT_COUNTER(void) == 1);
static_assert(CURRENT_COUNTER(void) == 1);
INC_COUNTER(void);
static_assert(CURRENT_COUNTER(void) == 2);
static_assert(CURRENT_COUNTER(void) == 2);
