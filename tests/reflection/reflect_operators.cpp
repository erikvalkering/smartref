#include <reflection/reflection.h>

// TODO:
// - REFLECTABLE_OPERATOR
// - REFLECTABLE_OPERATOR_UNARY
// - REFLECTABLE_OPERATOR_ASSIGNMENT

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

REFLECTION_REFLECTABLE_NONINTRUSIVE_ASSIGNMENT_OPERATOR(auto, operator=, member_function);

// TODO: operator,
// #define REFLECTION_OPERATOR_COMMA operator,
// REFLECTABLE(auto, REFLECTION_OPERATOR_COMMA,  member_function);
