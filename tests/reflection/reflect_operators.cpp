#include <reflection/reflection.h>

REFLECTABLE(auto, operator!=,   member_function);
REFLECTABLE(auto, operator%,    member_function);
REFLECTABLE(auto, operator%=,   member_function);
REFLECTABLE(auto, operator&&,   member_function);
REFLECTABLE(auto, operator&,    member_function);
REFLECTABLE(auto, operator&=,   member_function);
REFLECTABLE(auto, operator(),   member_function);
REFLECTABLE(auto, operator*,    member_function);
REFLECTABLE(auto, operator*=,   member_function);
REFLECTABLE(auto, operator++,   member_function);
REFLECTABLE(auto, operator+,    member_function);
REFLECTABLE(auto, operator+=,   member_function);
REFLECTABLE(auto, operator-,    member_function);
REFLECTABLE(auto, operator--,   member_function);
REFLECTABLE(auto, operator-=,   member_function);
REFLECTABLE(auto, operator->*,  member_function);
REFLECTABLE(auto, operator/,    member_function);
REFLECTABLE(auto, operator/=,   member_function);
REFLECTABLE(auto, operator<,    member_function);
REFLECTABLE(auto, operator<<,   member_function);
REFLECTABLE(auto, operator<<=,  member_function);
REFLECTABLE(auto, operator<=,   member_function);
REFLECTABLE(auto, operator=,    member_function);
REFLECTABLE(auto, operator==,   member_function);
REFLECTABLE(auto, operator>,    member_function);
REFLECTABLE(auto, operator>=,   member_function);
REFLECTABLE(auto, operator>>,   member_function);
REFLECTABLE(auto, operator>>=,  member_function);
REFLECTABLE(auto, operator[],   member_function);
REFLECTABLE(auto, operator^,    member_function);
REFLECTABLE(auto, operator^=,   member_function);
REFLECTABLE(auto, operator|,    member_function);
REFLECTABLE(auto, operator|=,   member_function);
REFLECTABLE(auto, operator||,   member_function);

REFLECTION_REFLECTABLE_NONINTRUSIVE_UNARY_OPERATOR(auto, operator!,   member_function);
REFLECTION_REFLECTABLE_NONINTRUSIVE_UNARY_OPERATOR(auto, operator->,  member_function);
REFLECTION_REFLECTABLE_NONINTRUSIVE_UNARY_OPERATOR(auto, operator~,   member_function);

// TODO: operator,
// #define REFLECTION_OPERATOR_COMMA operator,
// REFLECTABLE(auto, REFLECTION_OPERATOR_COMMA,  member_function);
