#include "counter.h"

static_assert(CURRENT_COUNTER(void) == 0);
static_assert(CURRENT_COUNTER(void) == 0);
INC_COUNTER(void);
static_assert(CURRENT_COUNTER(void) == 1);
static_assert(CURRENT_COUNTER(void) == 1);
INC_COUNTER(void);
static_assert(CURRENT_COUNTER(void) == 2);
static_assert(CURRENT_COUNTER(void) == 2);

struct CounterTest
{
    static_assert(CURRENT_CLASS_COUNTER() == 0);
    static_assert(CURRENT_CLASS_COUNTER() == 0);
    INC_CLASS_COUNTER();
    static_assert(CURRENT_CLASS_COUNTER() == 1);
    static_assert(CURRENT_CLASS_COUNTER() == 1);
    INC_CLASS_COUNTER();
    static_assert(CURRENT_CLASS_COUNTER() == 2);
    static_assert(CURRENT_CLASS_COUNTER() == 2);
};

struct DerivedTest : CounterTest
{
    static_assert(CURRENT_CLASS_COUNTER() == 2);
    static_assert(CURRENT_CLASS_COUNTER() == 2);
    INC_CLASS_COUNTER();
    static_assert(CURRENT_CLASS_COUNTER() == 3);
    static_assert(CURRENT_CLASS_COUNTER() == 3);
    INC_CLASS_COUNTER()
    static_assert(CURRENT_CLASS_COUNTER() == 4);
    static_assert(CURRENT_CLASS_COUNTER() == 4);
};

int main()
{
}
